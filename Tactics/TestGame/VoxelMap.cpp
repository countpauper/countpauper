#include "pch.h"
#include <numeric>
#include <cfenv>
#include "Game/VoxelMap.h"
#include "Engine/Maths.h"
#include "Engine/AxisAlignedBoundingBox.h"
#include "Data.h"

namespace Game
{
namespace Test
{

TEST(VoxelMap, Space)
{
    VoxelMap map;
    map.Space(1, 1, 1);
    EXPECT_FALSE(map.At(Position(0, 0, 0)));
    EXPECT_FALSE(map.At(Position(1, 0, 0)));
    EXPECT_FALSE(map.At(Position(0, -1, 0)));
    EXPECT_FALSE(map.At(Position(0, 0, -1)));    
    EXPECT_EQ(0, map.Mass());
}

TEST(VoxelData, Construction)
{
    VoxelMap::Data data(1, 2, 3);
    EXPECT_EQ(1, data.Longitude());
    EXPECT_EQ(2, data.Latitude());
    EXPECT_EQ(3, data.Altitude());
}

TEST(VoxelData, Indexing)
{
    VoxelMap::Data data(2, 2, 2);
    EXPECT_EQ(&VoxelMap::Material::vacuum, data[Position(1, 0, 1)].material );
    EXPECT_EQ(0, data[Position(-1, 2, 0)].density);
    EXPECT_THROW(data[Position(3, 0, 1)], std::out_of_range);
    EXPECT_THROW(data[Position(1, -2, 0)], std::out_of_range);
}

TEST(VoxelData, All)
{
    VoxelMap::Data data(3, 2, 1);
    auto all = data.All();
    EXPECT_EQ(Position(5, 4, 3), all.end() - all.begin());

}

TEST(VoxelData, IsInside)
{
    VoxelMap::Data data(3, 2, 1);
    EXPECT_TRUE(data.IsInside(Position(0, 0, 0)));
    EXPECT_TRUE(data.IsInside(Position(2, 1, 0)));
    EXPECT_FALSE(data.IsInside(Position(0, -1, 0)));
    EXPECT_FALSE(data.IsInside(Position(1, 1, 1)));

    EXPECT_TRUE(std::all_of(data.begin(), data.end(), [&data](const decltype(data)::value_type& v)
    {
        return data.IsInside(v.first);
    }));
}

TEST(VoxelData, Section)
{
    VoxelMap::Data data(3, 2, 1);
    Engine::AABB bounds(Engine::Range<double>(1, 3), Engine::Range<double>(-5, 5), Engine::Range<double>(0, 1));
    auto section = data.In(bounds);
    EXPECT_TRUE(std::all_of(section.begin(), section.end(), [&bounds](const decltype(data)::value_type& v)
    {
        return bounds.Contains(VoxelMap::Center(v.first));
    }));
}

TEST(VoxelData, NoBoundary)
{
    VoxelMap::Data data(3, 2, 1);
    auto middle = data.BoundaryCondition(Direction::none);
    for (const auto& v : middle)
    {
        EXPECT_TRUE(data.IsInside(v.first));
        EXPECT_TRUE(data.IsBoundary(v.first).empty());
    }
}

TEST(VoxelData, Boundary)
{
    VoxelMap::Data data(3, 2, 1);
    for (auto dir : Direction::all)
    {
        auto boundary = data.BoundaryCondition(dir);
        for (const auto& v : boundary)
        {
            EXPECT_FALSE(data.IsInside(v.first));
            EXPECT_EQ(Directions(dir), data.IsBoundary(v.first));
            EXPECT_TRUE(data.IsInside(v.first - dir.Vector()));
        }
    }
}

TEST(VoxelData, Iterator)
{
    VoxelMap::Data data(2, 2, 2);
    unsigned count = 0;
    for (const auto& v : data)
        ++count;
    EXPECT_EQ(8, count);
    EXPECT_EQ(Position(2, 2, 2), data.end() - data.begin());
    auto first = *data.begin();
    EXPECT_EQ(Position(0, 0, 0), first.first);
    EXPECT_FALSE(data.IsBoundary(first.first));
}

TEST(VoxelData, BoundaryGrids)
{
    VoxelMap::Data data(2, 2, 2);
    EXPECT_EQ(Direction::east | Direction::north | Direction::up, data.IsBoundary(Position(2, 2, 2)));
    EXPECT_EQ(Direction::west | Direction::south | Direction::down, data.IsBoundary(Position(-1, -1, -1)));
}

TEST(VoxelFlux, Index)
{
    VoxelMap::Data data(3, 2, 1);
    // Flux can be indexed in the center and is initialized to 0 
    EXPECT_EQ(0, data.U()[Position(1, 1, 0)]);
   
    // Flux can be indexed on the boundary conditions and is initialized to 0
    EXPECT_EQ(0, data.U()[Position(1, 1, 0)]);
    EXPECT_EQ(0, data.U()[Position(1, -1, 0)]);
    EXPECT_EQ(0, data.U()[Position(1, 0, -1)]);
    EXPECT_EQ(0, data.U()[Position(1, 2, 0)]);
    EXPECT_EQ(0, data.U()[Position(1, 0, 1)]);
    // Flux outside of the boundaries throws an out of range 
    EXPECT_THROW(data.U()[Position(-1, 0, 0)], std::out_of_range);
    EXPECT_THROW(data.U()[Position(1, 3, 0)], std::out_of_range);
    EXPECT_THROW(data.U()[Position(1, 0, -2)], std::out_of_range);
    // Flux on the boundary corners is initialized to nan
    EXPECT_TRUE(std::isnan(data.U()[Position(0, -1, 1)]));
    EXPECT_TRUE(std::isnan(data.U()[Position(0, 2, 1)]));
    EXPECT_TRUE(std::isnan(data.U()[Position(3, 2, -1)]));
    EXPECT_TRUE(std::isnan(data.U()[Position(1, -1, -1)]));
    EXPECT_TRUE(std::isnan(data.U()[Position(3, 0, -1)]));

    auto n = std::numeric_limits<double>::signaling_NaN();
    
    unsigned int control_word;
    auto err = _controlfp_s(&control_word, EM_INVALID, MCW_EM);
    ASSERT_FALSE(err);
    // TODO: how to get signal nan to signal?
}

TEST(VoxelFlux, Iterator)
{   // Along the flux axis, position 0 and (long/lat/alt) is a boundary condition
    // only in between grids is there dynamic flux
    // along the other axes the flux is defind in the center of the grid cells, 
    // so every dynamic grid has 1s
    VoxelMap::Data data(3, 2, 1);
    const auto& u = data.U();
    EXPECT_EQ(Position(2, 2, 1), u.end() - u.begin());
    EXPECT_EQ(Position(1, 0, 0), u.begin().position);
    const auto& v = data.V();
    EXPECT_EQ(Position(3, 1, 1), v.end() - v.begin());
    EXPECT_EQ(Position(0, 1, 0), v.begin().position);
    // Due to only being altitiude==1, there is no free flux in the w direction, 
    // to prevent iterating begin is set to end for operator==
    const auto& w = data.W();
    EXPECT_EQ(Position(0, 0, 0), w.end() - w.begin());
}

TEST(VoxelFlux, Boundary)
{
    // Along the flux axis, position 0 and long/lat alt are the boundary conditions
    // along the other axes, the boundary condtions are the dimensions of the dynamic grid
    // Flux at the corners is not defined
    VoxelMap::Data data(3, 2, 1);
    auto west = data.U().BoundaryCondition(Direction::west);
    EXPECT_EQ(Position(1, 2, 1), west.end() - west.begin());
    EXPECT_EQ(Position(3, 0, 0), west.begin().position);
    EXPECT_TRUE(std::all_of(west.begin(), west.end(), [](const decltype(west)::value_type& fluxPair)->bool
    {
        return fluxPair.first.x == 3 && fluxPair.second == 0.0;
    }));
    auto east = data.U().BoundaryCondition(Direction::east);
    EXPECT_EQ(Position(1, 2, 1), east.end() - east.begin());
    EXPECT_EQ(Position(0, 0, 0), east.begin().position);
    EXPECT_TRUE(std::all_of(east.begin(), east.end(), [](const decltype(east)::value_type& fluxPair)->bool
    {
        return fluxPair.first.x == 0 && fluxPair.second == 0.0;
    }));
    auto south = data.U().BoundaryCondition(Direction::south);
    EXPECT_EQ(Position(2, 1, 1), south.end() - south.begin());
    EXPECT_EQ(Position(1, 2, 0), south.begin().position);
    EXPECT_TRUE(std::all_of(south.begin(), south.end(), [](const decltype(south)::value_type& fluxPair)->bool
    {
        return ((fluxPair.first.y == 2) &&
            (Engine::Range<int>(0, 3)[fluxPair.first.x]) &&
            (Engine::Range<int>(0, 1)[fluxPair.first.z]));
    }));
    auto up = data.U().BoundaryCondition(Direction::up);
    EXPECT_EQ(Position(2, 2, 1), up.end() - up.begin());
    EXPECT_EQ(Position(1, 0, -1), up.begin().position);
    EXPECT_TRUE(std::all_of(up.begin(), up.end(), [](const decltype(up)::value_type& fluxPair)->bool
    {
        return fluxPair.first.z == -1 && fluxPair.second == 0.0;
    }));
}

TEST(VoxelMap, Air)
{
    VoxelMap map;
    map.Space(2, 2, 2);
    map.Air(300, 20000);
    EXPECT_EQ(Element::Air, map.At(Position(0, 0, 0)).floor);
    EXPECT_FALSE(map.At(Position(0, 0, 0)).solid);
    EXPECT_EQ(Element::Air, map.At(Position(1, 1, 1)).floor);
    EXPECT_EQ(0, map.At(Position(1, 1, 1)).height);
    EXPECT_NEAR(1.17 * map.Volume(), map.Mass(), map.Volume()*0.01);
    EXPECT_NEAR(300, map.Temperature(VoxelMap::Material::air), 0.01);
}

TEST(VoxelMap, Lava)
{
    VoxelMap map;
    map.Space(2, 2, 2);
    map.World(10e6);

    EXPECT_EQ(Element::Fire, map.At(Position(0, 0, 0)).floor);
    EXPECT_FALSE(map.At(Position(0, 0, 0)).solid);
    EXPECT_EQ(Element::Fire, map.At(Position(1, 1, 1)).floor);
    EXPECT_EQ(0, map.At(Position(1, 1, 1)).height);
}

TEST(VoxelMap, Water)
{
    VoxelMap map;
    map.Space(2, 2, 2);
    map.Water(1, 300);

    EXPECT_EQ(Element::Water, map.At(Position(0, 0, 0)).floor);
    EXPECT_FALSE(map.At(Position(0, 0, 0)).solid);
    EXPECT_EQ(Element::Water, map.At(Position(1, 1, 1)).floor);
    EXPECT_EQ(0, map.At(Position(1, 1, 2)).height);
}

TEST(VoxelMap, HillPeak)
{
    VoxelMap map;
    map.Space(3, 3, 3);
    map.Air(300, 10000);
    map.Hill(Engine::Line(Engine::Coordinate(1.5, 1.5, 2), Engine::Coordinate(1.5, 1.5, 2)), 1.0f/float(Engine::FullWidthHalfMaximum(1)));

    EXPECT_EQ(Element::Stone, map.At(Position(1, 1, 2)).floor);
    EXPECT_EQ(2, map.At(Position(1, 1, 3)).height);
    EXPECT_EQ(Element::Stone, map.At(Position(1, 0, 2)).floor);
    EXPECT_EQ(1, map.At(Position(1, 0, 3)).height);
}

TEST(VoxelMap, HillRidge)
{
    VoxelMap map;
    map.Space(3, 3, 3);
    map.Air(300, 10000);
    map.World(10e6);
    map.Hill(Engine::Line(Engine::Coordinate(3, 0, 0), Engine::Coordinate(0, 3, 3)), 1.666 );
    // The line goes straight through the middle of (0,2) (1,1) and (2,0)
    // but is highest at the first and lowest at the last
    EXPECT_EQ(Element::Stone, map.At(Position(2, 0, 3)).floor);
    EXPECT_EQ(1, map.At(Position(2, 0, 3)).height);

    EXPECT_EQ(2, map.At(Position(1, 1, 2)).height);
    EXPECT_EQ(Element::Stone, map.At(Position(1, 1, 1)).floor);

    EXPECT_EQ(Element::Stone, map.At(Position(0, 2, 3)).floor);
    EXPECT_EQ(3, map.At(Position(0, 2, 3)).height);

    // 0,0 is one diagonal (roughly grid off the middle of the ridge line)
    //  which is close to full width half maximum of 1.666, so it should be one lower 
    EXPECT_EQ(Element::Stone, map.At(Position(0, 0, 3)).floor);
    EXPECT_EQ(1, map.At(Position(0, 0, 3)).height);
}

TEST(VoxelMap, StraightWall)
{
    VoxelMap map;
    map.Space(3, 3, 3);
    map.Wall(Engine::Line(Engine::Coordinate(1.5, -1, 0), Engine::Coordinate(1.5, 4, 0)), 2.0, 1.0);

    // Check along the wall
    EXPECT_EQ(Element::Stone, map.At(Position(1, 0, 3)).floor);
    EXPECT_EQ(2, map.At(Position(1, 0, 3)).height);
    EXPECT_EQ(Element::Stone, map.At(Position(1, 1, 3)).floor);
    EXPECT_EQ(2, map.At(Position(1, 1, 3)).height);
    EXPECT_EQ(Element::Stone, map.At(Position(1, 2, 3)).floor);
    EXPECT_EQ(2, map.At(Position(1, 2, 3)).height);

    // Next to the wall
    EXPECT_EQ(Element::None, map.At(Position(0, 0, 3)).floor);
    EXPECT_EQ(0, map.At(Position(0, 0, 3)).height);
    EXPECT_EQ(Element::None, map.At(Position(0, 0, 3)).floor);
    EXPECT_EQ(0, map.At(Position(2, 2, 3)).height);
}


}
}
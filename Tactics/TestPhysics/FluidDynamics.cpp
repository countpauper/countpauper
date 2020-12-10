#include "pch.h"
#include "Physics/FluidDynamics.h"

namespace Physics
{
namespace Test
{


TEST(VoxelFlux, Index)
{
    FluidDynamics::Data data(Size(3, 2, 1));
    // Flux can be indexed in the center and is initialized to 0 
    EXPECT_EQ(0, data.U()[Position(0, 1, 0)]);

    // Flux can be indexed on the boundary conditions and is initialized to 0
    EXPECT_EQ(0, data.U()[Position(0, 1, 0)]);
    EXPECT_EQ(0, data.U()[Position(0, -1, 0)]);
    EXPECT_EQ(0, data.U()[Position(0, 0, -1)]);
    EXPECT_EQ(0, data.U()[Position(0, 2, 0)]);
    EXPECT_EQ(0, data.U()[Position(0, 0, 1)]);
    // Flux on the boundary corners is initialized to 0 as well 
    EXPECT_EQ(0, data.U()[Position(-1, -1, 1)]);
    EXPECT_EQ(0, data.U()[Position(-1, 2, 1)]);
    EXPECT_EQ(0, data.U()[Position(2, 2, -1)]);
    EXPECT_EQ(0, data.U()[Position(0, -1, -1)]);
    EXPECT_EQ(0, data.U()[Position(2, 0, -1)]);
    // Flux outside of the boundaries throws an out of range 
    EXPECT_THROW(data.U()[Position(-2, 0, 0)], std::out_of_range);
    EXPECT_THROW(data.U()[Position(3, 0, 0)], std::out_of_range);
    EXPECT_THROW(data.U()[Position(0, 3, 0)], std::out_of_range);
    EXPECT_THROW(data.U()[Position(0, 0, -2)], std::out_of_range);
}

TEST(VoxelFlux, Iterator)
{   // Along the flux axis, position 0 and (long/lat/alt) is a boundary condition
    // only in between grids is there dynamic flux
    // along the other axes the flux is defind in the center of the grid cells, 
    // so every dynamic grid has 1s
    FluidDynamics::Data data(Size(3, 2, 1));
    const auto& u = data.U();
    EXPECT_EQ(Position(2, 2, 1), u.end() - u.begin());
    EXPECT_EQ(Position(0, 0, 0), u.begin().position);
    const auto& v = data.V();
    EXPECT_EQ(Position(3, 1, 1), v.end() - v.begin());
    EXPECT_EQ(Position(0, 0, 0), v.begin().position);
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
    FluidDynamics::Data data(Size(3, 2, 1));
    auto west = data.U().BoundaryCondition(Direction::west);
    EXPECT_EQ(Position(1, 2, 1), west.end() - west.begin());
    EXPECT_EQ(Position(2, 0, 0), west.begin().position);
    EXPECT_TRUE(std::all_of(west.begin(), west.end(), [](const decltype(west)::value_type& fluxPair)->bool
    {
        return fluxPair.first.x == 2 && fluxPair.second == 0.0;
    }));
    auto east = data.U().BoundaryCondition(Direction::east);
    EXPECT_EQ(Position(1, 2, 1), east.end() - east.begin());
    EXPECT_EQ(Position(-1, 0, 0), east.begin().position);
    EXPECT_TRUE(std::all_of(east.begin(), east.end(), [](const decltype(east)::value_type& fluxPair)->bool
    {
        return fluxPair.first.x == -1 && fluxPair.second == 0.0;
    }));
    auto south = data.U().BoundaryCondition(Direction::south);
    EXPECT_EQ(Position(2, 1, 1), south.end() - south.begin());
    EXPECT_EQ(Position(0, 2, 0), south.begin().position);
    EXPECT_TRUE(std::all_of(south.begin(), south.end(), [](const decltype(south)::value_type& fluxPair)->bool
    {
        return ((fluxPair.first.y == 2) &&
            (Engine::Range<int>(0, 3)[fluxPair.first.x]) &&
            (Engine::Range<int>(0, 1)[fluxPair.first.z]));
    }));
    auto up = data.U().BoundaryCondition(Direction::up);
    EXPECT_EQ(Position(2, 2, 1), up.end() - up.begin());
    EXPECT_EQ(Position(0, 0, -1), up.begin().position);
    EXPECT_TRUE(std::all_of(up.begin(), up.end(), [](const decltype(up)::value_type& fluxPair)->bool
    {
        return fluxPair.first.z == -1 && fluxPair.second == 0.0;
    }));
}

TEST(VoxelFlux, BoundaryEdge)
{
    FluidDynamics::Data data(Physics::Size(3, 2, 1));
    auto north_west = data.U().BoundaryCondition(Direction::west | Direction::north);
    EXPECT_EQ(Position(1, 1, 3), north_west.end() - north_west.begin());
    EXPECT_EQ(Position(2, -1, -1), north_west.begin().position);
    EXPECT_TRUE(std::all_of(north_west.begin(), north_west.end(), [](const decltype(north_west)::value_type& fluxPair)->bool
    {
        return fluxPair.first.x == 2 && fluxPair.first.y == -1 && fluxPair.second == 0.0;
    }));
    auto north_west_down = data.U().BoundaryCondition(Direction::west | Direction::north | Direction::down);
    EXPECT_EQ(Position(1, 1, 1), north_west_down.end() - north_west_down.begin());
    EXPECT_EQ(Position(2, -1, 1), north_west_down.begin().position);
    EXPECT_EQ(0.0, (*north_west_down.begin()).second);

    auto east_west_youre_stressed = data.U().BoundaryCondition(Direction::west | Direction::east);
    EXPECT_EQ(east_west_youre_stressed.end(), east_west_youre_stressed.begin());
}

}
}
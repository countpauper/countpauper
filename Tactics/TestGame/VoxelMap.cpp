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
    map.Space(Physics::Size(1, 1, 1));
    EXPECT_FALSE(map.At(Position(0, 0, 0)));
    EXPECT_FALSE(map.At(Position(1, 0, 0)));
    EXPECT_FALSE(map.At(Position(0, -1, 0)));
    EXPECT_FALSE(map.At(Position(0, 0, -1)));    
}

TEST(VoxelMap, Air)
{
    VoxelMap map;
    map.Space(Physics::Size(2, 2, 2));
    map.Air(300, 20000);
    EXPECT_EQ(Element::Air, map.At(Position(0, 0, 0)).floor);
    EXPECT_FALSE(map.At(Position(0, 0, 0)).solid);
    EXPECT_EQ(Element::Air, map.At(Position(1, 1, 1)).floor);
    EXPECT_EQ(0, map.At(Position(1, 1, 1)).height);
}

TEST(VoxelMap, Sea)
{
    VoxelMap map;
    map.Space(Physics::Size(2, 2, 2));
    map.Sea(0.33, 300);

    EXPECT_EQ(Element::Water, map.At(Position(0, 0, 0)).floor);
    EXPECT_FALSE(map.At(Position(0, 0, 0)).solid);
    EXPECT_EQ(Element::Water, map.At(Position(1, 1, 1)).floor);
    EXPECT_EQ(1, map.At(Position(1, 1, 1)).height);
}

TEST(VoxelMap, HillPeak)
{
    VoxelMap map;
    map.Space(Physics::Size(3, 3, 3));
    map.Air(300, 10000);
    map.Hill(Engine::Line(Engine::Coordinate(1.5, 1.5, 2/3.0), Engine::Coordinate(1.5, 1.5, 2 / 3.0)), 1.0f/float(Engine::FullWidthHalfMaximum(1)));

    EXPECT_EQ(Element::Stone, map.At(Position(1, 1, 2)).floor);
    EXPECT_EQ(2, map.At(Position(1, 1, 3)).height);
    EXPECT_EQ(Element::Stone, map.At(Position(1, 0, 2)).floor);
    EXPECT_EQ(1, map.At(Position(1, 0, 3)).height);
}

TEST(VoxelMap, HillRidge)
{
    VoxelMap map;
    map.Space(Physics::Size(4, 4, 4));
    map.Air(300, 10000);
    map.World(10e6);
    map.Hill(Engine::Line(Engine::Coordinate(3, 0, 0), Engine::Coordinate(0, 3, 3/3.0)), 1.666 );
    // The line goes straight through the middle of (0,2) (1,1) and (2,0)
    // but is highest at the first and lowest at the last

    EXPECT_EQ(Element::Stone, map.At(Position(2, 0, 3)).floor);
    EXPECT_EQ(0, map.At(Position(3, 0, 3)).height);
    EXPECT_EQ(3, map.At(Position(0, 3, 3)).height);

    EXPECT_EQ(2, map.At(Position(1, 1, 2)).height);
    EXPECT_EQ(Element::Stone, map.At(Position(1, 1, 1)).floor);

    EXPECT_EQ(Element::Stone, map.At(Position(0, 2, 3)).floor);
    EXPECT_EQ(3, map.At(Position(0, 2, 3)).height);

    // 0,0 is one diagonal (roughly grid off the middle of the ridge line)
    //  which is close to full width half maximum of 1.666, so it should be one lower 
    EXPECT_EQ(Element::Stone, map.At(Position(0, 0, 3)).floor);
    EXPECT_EQ(1, map.At(Position(0, 0, 3)).height);
}


TEST(VoxelMap, UnitWall)
{
    VoxelMap map;
    map.Space(Physics::Size(2, 2, 2));
    map.Wall(Engine::Line(Engine::Coordinate(0, 0.5, 0), Engine::Coordinate(1, 0.5, 0)), 1.0/3.0, 1.0);

    // Check along the wall
    EXPECT_EQ(Element::Stone, map.At(Position(0, 0, 2)).floor);
    EXPECT_EQ(1, map.At(Position(0, 0, 2)).height);
    
    // Next to the wall
    EXPECT_EQ(Element::None, map.At(Position(1, 0, 2)).floor);
    EXPECT_EQ(0, map.At(Position(1, 0, 2)).height);
    EXPECT_EQ(Element::None, map.At(Position(0, 1, 2)).floor);
    EXPECT_EQ(0, map.At(Position(0, 1, 2)).height);
    EXPECT_EQ(Element::None, map.At(Position(1, 1, 2)).floor);
    EXPECT_EQ(0, map.At(Position(1, 1, 2)).height);
}

TEST(VoxelMap, StraightWall)
{
    VoxelMap map;
    map.Space(Physics::Size(3, 3, 3));
    map.Wall(Engine::Line(Engine::Coordinate(1.5, 0, 0), Engine::Coordinate(1.5, 2, 0)), 2.0 / 3.0, 1.0);

    // Check along the wall
    EXPECT_EQ(Element::Stone, map.At(Position(1, 0, 3)).floor);
    EXPECT_EQ(2, map.At(Position(1, 0, 3)).height);
    EXPECT_EQ(Element::Stone, map.At(Position(1, 1, 3)).floor);
    EXPECT_EQ(2, map.At(Position(1, 1, 3)).height);
    EXPECT_EQ(Element::None, map.At(Position(1, 2, 3)).floor);
    EXPECT_EQ(0, map.At(Position(1, 2, 3)).height);

    // Next to the wall
    EXPECT_EQ(Element::None, map.At(Position(0, 0, 3)).floor);
    EXPECT_EQ(0, map.At(Position(0, 0, 3)).height);
    EXPECT_EQ(Element::None, map.At(Position(2, 2, 3)).floor);
    EXPECT_EQ(0, map.At(Position(2, 2, 3)).height);
}

TEST(VoxelMap, DiagonalWall)
{
    VoxelMap map;
    map.Space(Physics::Size(3, 3, 3));
    map.Wall(Engine::Line(Engine::Coordinate(2.5, 0, 0), Engine::Coordinate(0, 1.5, 0)), 2.0 / 3.0, 1.0);

    // Check along the wall
    EXPECT_EQ(Element::Stone, map.At(Position(2, 0, 3)).floor);
    EXPECT_EQ(Element::Stone, map.At(Position(1, 0, 3)).floor);
    EXPECT_EQ(Element::Stone, map.At(Position(0, 1, 3)).floor);
    // Next to the wall
    EXPECT_EQ(Element::None, map.At(Position(0, 0, 3)).floor);
    EXPECT_EQ(Element::None, map.At(Position(1, 1, 3)).floor);
}


TEST(VoxelMap, SlantedWall)
{
    VoxelMap map;
    map.Space(Physics::Size(3, 3, 3));
    map.Wall(Engine::Line(Engine::Coordinate(1.5, 0, 0), Engine::Coordinate(1.5, 3, 2/3.0)), 1.0/3.0, 1.0);

    // Check along the wall
    EXPECT_EQ(1, map.At(Position(1, 0, 3)).height); // 1.333
    EXPECT_EQ(2, map.At(Position(1, 1, 3)).height); // 2.000
    EXPECT_EQ(3, map.At(Position(1, 2, 3)).height); // 2.666
    EXPECT_EQ(0, map.At(Position(1, 3, 3)).height); // 0

    // Next to the wall
    EXPECT_EQ(0, map.At(Position(0, 0, 3)).height);
}


TEST(VoxelMap, Tick)
{
    VoxelMap map;
    map.Space(Physics::Size(1, 1, 1));
    map.Air(300, 10000);
    map.Tick(0);
    map.Tick(0.1);
    EXPECT_EQ(Element::Air, map.At(Position(0, 0, 0)).floor);
}

}
}
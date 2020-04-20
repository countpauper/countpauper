#include "pch.h"
#include <numeric>
#include "Game/VoxelMap.h"
#include "Data.h"

namespace Game
{
namespace Test
{

TEST(Map, Space)
{
    VoxelMap map;
    map.Space(1, 1, 1);
    EXPECT_FALSE(map.At(Position(0, 0, 0)));
    EXPECT_FALSE(map.At(Position(1, 0, 0)));
    EXPECT_FALSE(map.At(Position(0, -1, 0)));
    EXPECT_TRUE(map.At(Position(0, 0, -1)));    // lava
}

TEST(Map, Air)
{
    VoxelMap map;
    map.Space(2, 2, 2);
    map.Air(300);
    EXPECT_EQ(Element::Fire, map.At(Position(0, 0, 0)).floor);
    EXPECT_FALSE(map.At(Position(0, 0, 0)).solid);
    EXPECT_EQ(Element::Fire, map.At(Position(1, 1, 1)).floor);
    EXPECT_EQ(0, map.At(Position(1, 1, 1)).height);
}

TEST(Map, Water)
{
    VoxelMap map;
    map.Space(2, 2, 2);
    map.Air(300);
    map.Water(1, 300);

    EXPECT_EQ(Element::Fire, map.At(Position(0, 0, 0)).floor);
    EXPECT_FALSE(map.At(Position(0, 0, 0)).solid);
    EXPECT_EQ(Element::Fire, map.At(Position(1, 1, 1)).floor);
    EXPECT_EQ(0, map.At(Position(1, 1, 1)).height);
}


}
}
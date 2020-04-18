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
    EXPECT_FALSE(map.At(Position(0, 0)));
    EXPECT_FALSE(map.At(Position(1, 0)));
    EXPECT_FALSE(map.At(Position(0, -1)));
}


}
}
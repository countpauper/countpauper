#include "pch.h"
#include "Game/Position.h"

namespace Game
{
namespace Test
{


TEST(Position, Bool)
{
    EXPECT_FALSE(Position());
    EXPECT_TRUE(Position(1, 0, 0));
}

TEST(Position, Size)
{
    EXPECT_EQ(0, Position().Size());
    EXPECT_EQ(1.0, Position(1, 0, 0).Size());
    EXPECT_EQ(1.0, Position(0, -1, 0).Size());
}

TEST(Position, ManhattanDistance)
{
    EXPECT_EQ(0, Position(1, 1, 0).ManDistance(Position(1, 1, 0)));
    EXPECT_EQ(1, Position(0, 1, 0).ManDistance(Position(1, 1, 0)));
    EXPECT_EQ(2, Position(0, 1, 0).ManDistance(Position(1, 0, 0)));
    EXPECT_EQ(3, Position(0, 1, 0).ManDistance(Position(1, -1, 0)));
    EXPECT_EQ(2, Position(0, 1, 0).ManDistance(Position(0, 0, 1)));
    EXPECT_EQ(3, Position(0, 1, 0).ManDistance(Position(0, -1, -1)));
}



TEST(Position, ElDistance)
{   // A 1x1 square is 3x3 el, diagonals are rounded to +1 for 45 degrees
    EXPECT_EQ(0, Position(1, 1, 0).DistanceEl(Position(1, 1, 0))); // 0,0, 0
    EXPECT_EQ(1, Position(1, 1, 0).DistanceEl(Position(1, 1, 1))); // 0,0, 1
    EXPECT_EQ(3, Position(0, 1, 0).DistanceEl(Position(1, 1, 0))); // 1,0, 0
    EXPECT_EQ(3, Position(0, 1, 0).DistanceEl(Position(0, 0, 0))); // 0,-1, 0
    EXPECT_EQ(3, Position(0, 1, 0).DistanceEl(Position(1, 1, 1))); // 1, 0, 1
    EXPECT_EQ(4, Position(0, 1, 0).DistanceEl(Position(1, 1, 2))); // 1, 0, 2
    EXPECT_EQ(4, Position(0, 1, 0).DistanceEl(Position(1, 0, 0))); // 1,-1, 0
    EXPECT_EQ(6, Position(0, 1, 0).DistanceEl(Position(0, 3, 0))); // 0,2, 0
    EXPECT_EQ(7, Position(0, 1, 0).DistanceEl(Position(1, 3, 0))); // 1,2, 0
    EXPECT_EQ(8, Position(-1, 1, 0).DistanceEl(Position(1, 3, 0))); //2,2, 0    
    EXPECT_EQ(9, Position(-1, 1, 0).DistanceEl(Position(2, 1, 0))); // 3,0, 0
    EXPECT_EQ(10, Position(-1, 1, 0).DistanceEl(Position(0, 4, 0))); // 1,3, 0 
    EXPECT_EQ(11, Position(-1, 1, 0).DistanceEl(Position(2, 3, 0))); // 3,2, 0
    EXPECT_EQ(13, Position(-1, 1, 0).DistanceEl(Position(2, 4, 0))); // 3,3, 0
}

}
}  // Test
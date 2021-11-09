#include "pch.h"
#include <numeric>
#include "Game/Map.h"
#include "Data.h"

namespace Game
{
namespace Test
{

TEST(Map, At)
{
    Data::MiniMap map;
    EXPECT_EQ(Element::Stone, map.At(Position(0, 0, 1)).floor);
    EXPECT_EQ(2, map.At(Position(0, 0, 1)).height);
    EXPECT_EQ(Element::Water, map.At(Position(1, 0, 0)).floor);
    EXPECT_EQ(1, map.At(Position(1, 0, 0)).height);
}

TEST(Map, CanBe)
{
    Data::MiniMap map;
    EXPECT_TRUE(map.CanBe(Position(0, 0, 1)));
    EXPECT_FALSE(map.CanBe(Position(1, 0, 0)));
}

TEST(Map, MaybeAt)
{
    Data::MiniMap map;
    EXPECT_EQ(Element::Stone, map.At(Position(0, 0, 1)).floor);
    EXPECT_EQ(2, map.At(Position(0, 0, 1)).height);

    EXPECT_FALSE(map.At(Position(-1, 0, 0)));
    EXPECT_FALSE(map.At(Position(0, -1, 0)));
    EXPECT_FALSE(map.At(Position(2, 0, 0)));
    EXPECT_FALSE(map.At(Position(0, 2, 0)));
}


TEST(Map, CanGo)
{
    Data::MiniMap map;
    EXPECT_FALSE(map.CanGo(Position(0, 0, 1), Physics::Direction::none));   // no direction 
    EXPECT_TRUE(map.CanGo(Position(0, 0, 1), Physics::Direction::north));   
    EXPECT_FALSE(map.CanGo(Position(0, 0, 1), Physics::Direction::south));  // south (0,-1) off of map
    // undefined if you can go to a square you can't be at EXPECT_FALSE(map.CanGo(Position(0, 0), Physics::Direction::east));
    EXPECT_FALSE(map.CanGo(Position(0, 0, 1), Physics::Direction::west));   // west(-1,0) of map

    EXPECT_TRUE(map.CanGo(Position(0, 0, 0), Physics::Direction::north));
    EXPECT_TRUE(map.CanGo(Position(0, 0, 0), Physics::Direction::east));
}

}
}  // Test
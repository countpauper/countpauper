#include "pch.h"
#include "Game/Direction.h"
#include "Game/Position.h"
#include "Engine/Geometry.h"

namespace Game
{
namespace Test
{


TEST(DirectionTest, None)
{
    EXPECT_TRUE(Direction().IsNone());
	EXPECT_TRUE(Direction::none.IsNone());
    EXPECT_FALSE(Direction::none.IsPosititve());
    EXPECT_FALSE(Direction::none.IsNegative());
}

TEST(DirectionTest, CardinalPositions)
{
    EXPECT_EQ(Direction(Position(0, 0, 0)), Direction::none);
    EXPECT_EQ(Direction(Position(0, -1, 0)), Direction::south);
    EXPECT_EQ(Direction(Position(0, 1, 0)), Direction::north);
    EXPECT_EQ(Direction(Position(-1, 0, 0)), Direction::west);
    EXPECT_EQ(Direction(Position(1, 0, 0)), Direction::east);
}


TEST(DirectionTest, DiagonalPositions)
{
	EXPECT_EQ(Direction(Position(-1, -1, 0)), Direction::west);
	EXPECT_EQ(Direction(Position(1, -1, 0)), Direction::south);
	EXPECT_EQ(Direction(Position(1, 1, 0)), Direction::east);
	EXPECT_EQ(Direction(Position(-1, 1, 0)), Direction::north);
}

TEST(DirectionTest, VerticalPositions)
{
	EXPECT_EQ(Direction(0, 0, 1), Direction::up);
	EXPECT_EQ(Direction(0, 0,-1), Direction::down);
	EXPECT_EQ(Direction(1, 1,-2), Direction::down);
	EXPECT_EQ(Direction(1, 0, 1), Direction::east);
}


TEST(DirectionTest, Opposite)
{
    EXPECT_TRUE(Direction::east.IsOpposite(Direction::west));
    EXPECT_TRUE(Direction::south.IsOpposite(Direction::north));
    EXPECT_TRUE(Direction::up.IsOpposite(Direction::down));
	EXPECT_FALSE(Direction::east.IsOpposite(Direction::north));
	EXPECT_FALSE(Direction::up.IsOpposite(Direction::west));
	EXPECT_FALSE(Direction::none.IsOpposite(Direction::west));
	EXPECT_FALSE(Direction::east.IsOpposite(Direction::east));

	EXPECT_TRUE(Direction::east.IsOpposite(Direction::east.Opposite()));
	EXPECT_TRUE(Direction::south.IsOpposite(Direction::south.Opposite()));
	EXPECT_TRUE(Direction::down.IsOpposite(Direction::down.Opposite()));
}

TEST(DirectionTest, PositiveNegative)
{
    EXPECT_TRUE(Direction::east.IsPosititve() ^ Direction::west.IsPosititve());
    EXPECT_TRUE(Direction::north.IsPosititve() ^ Direction::south.IsPosititve());
    EXPECT_TRUE(Direction::up.IsPosititve() ^ Direction::down.IsPosititve());
  
    EXPECT_TRUE(Direction::east.IsNegative() ^ Direction::west.IsNegative());
    EXPECT_TRUE(Direction::north.IsNegative() ^ Direction::south.IsNegative());
    EXPECT_TRUE(Direction::up.IsNegative() ^ Direction::down.IsNegative());

    for (auto dir : Direction::all)
    {
        EXPECT_TRUE(dir.IsPosititve() ^ dir.IsNegative()) << dir;
    }
}


TEST(DirectionTest, Angles)
{
	EXPECT_TRUE(std::isnan(Direction::none.Angle()));
	EXPECT_EQ(Direction::east.Angle(), 0);
	EXPECT_NEAR(Direction::north.Angle(), 0.5*Engine::PI, 0.001);
	EXPECT_NEAR(Direction::south.Angle(), -0.5*Engine::PI, 0.001);
	EXPECT_NEAR(Direction::west.Angle(), Engine::PI, 0.001);
	EXPECT_TRUE(std::isnan(Direction::up.Angle()));
	EXPECT_TRUE(std::isnan(Direction::down.Angle()));
}

TEST(DirectionTest, Clockwise)
{
	EXPECT_TRUE(Direction::east.IsClockwise(Direction::north));
	EXPECT_TRUE(Direction::south.IsClockwise(Direction::east));
	EXPECT_TRUE(Direction::west.IsClockwise(Direction::south));
	EXPECT_TRUE(Direction::north.IsClockwise(Direction::west));

	EXPECT_FALSE(Direction::west.IsClockwise(Direction::north));
	EXPECT_FALSE(Direction::north.IsClockwise(Direction::east));
	EXPECT_FALSE(Direction::east.IsClockwise(Direction::south));
	EXPECT_FALSE(Direction::south.IsClockwise(Direction::west));

	EXPECT_FALSE(Direction::east.IsClockwise(Direction::west));
	EXPECT_FALSE(Direction::south.IsClockwise(Direction::north));
	EXPECT_FALSE(Direction::none.IsClockwise(Direction::west));
	EXPECT_FALSE(Direction::up.IsClockwise(Direction::down));
	EXPECT_FALSE(Direction::down.IsClockwise(Direction::north));
}


TEST(DirectionTest, CounterClockwise)
{
	EXPECT_TRUE(Direction::west.IsCounterClockwise(Direction::north));
	EXPECT_TRUE(Direction::north.IsCounterClockwise(Direction::east));
	EXPECT_TRUE(Direction::east.IsCounterClockwise(Direction::south));
	EXPECT_TRUE(Direction::south.IsCounterClockwise(Direction::west));

	EXPECT_FALSE(Direction::east.IsCounterClockwise(Direction::north));
	EXPECT_FALSE(Direction::south.IsCounterClockwise(Direction::east));
	EXPECT_FALSE(Direction::west.IsCounterClockwise(Direction::south));
	EXPECT_FALSE(Direction::north.IsCounterClockwise(Direction::west));

	EXPECT_FALSE(Direction::east.IsCounterClockwise(Direction::west));
	EXPECT_FALSE(Direction::south.IsCounterClockwise(Direction::north));
	EXPECT_FALSE(Direction::none.IsCounterClockwise(Direction::west));
	EXPECT_FALSE(Direction::up.IsCounterClockwise(Direction::down));
	EXPECT_FALSE(Direction::down.IsCounterClockwise(Direction::north));
}

TEST(DirectionTest, EvenDivision)
{
    // even division of each direction in a 5x5 map
    // 4 directions x 6 places + 1 x none = 25 positions
    std::map<Direction, unsigned> count =
    {
        { Direction(Direction::none), 0 },
        { Direction(Direction::north), 0 },
        { Direction(Direction::east), 0 },
        { Direction(Direction::south), 0 },
        { Direction(Direction::west), 0 }
    };

    for (int y = -2; y <= 2; ++y)
    {
        for (int x = -2; x <= 2; ++x)
        {
            Direction dir(Position(x, y, 0));
            count[dir] += 1;
        }
    }
    EXPECT_TRUE(count[Direction::none] == 1);
    EXPECT_TRUE(count[Direction::north] == 6);
    EXPECT_TRUE(count[Direction::east] == 6);
    EXPECT_TRUE(count[Direction::south] == 6);
    EXPECT_TRUE(count[Direction::west] == 6);
}

TEST(DirectionTest, Vector)
{

	for (auto dir : Direction::all)
	{
		EXPECT_EQ(Direction(dir.Vector()), dir);
	}

}

TEST(DirectionTest, Prone)
{
	EXPECT_TRUE(Direction::up.IsProne());
	EXPECT_TRUE(Direction::down.IsProne());
	EXPECT_FALSE(Direction::east.IsProne());
	EXPECT_FALSE(Direction::north.IsProne());
	EXPECT_FALSE(Direction::none.IsProne());
}

TEST(DirectionTest, Turn)
{
	EXPECT_EQ(Direction::east.Turn(Direction::left), Direction::north);
	EXPECT_EQ(Direction::north.Turn(Direction::backward), Direction::south);
	EXPECT_EQ(Direction::south.Turn(Direction::right), Direction::west);
	EXPECT_EQ(Direction::west.Turn(Direction::forward), Direction::west);

	EXPECT_EQ(Direction::east.Turn(Direction::up), Direction::up);
	EXPECT_EQ(Direction::north.Turn(Direction::down), Direction::down);

	EXPECT_EQ(Direction::down.Turn(Direction::right), Direction::down);
	EXPECT_EQ(Direction::down.Turn(Direction::up), Direction::up);
}


TEST(Directions, Empty)
{   // Default initialized directions are empty
    Directions dirs; 
    EXPECT_FALSE(dirs);
    EXPECT_TRUE(dirs.empty());
    EXPECT_TRUE(dirs.begin() == dirs.end());
    EXPECT_FALSE(dirs[Direction::none]);
    EXPECT_FALSE(dirs[Direction::north]);
}

TEST(Directions, Single)
{   // Initializing with one directions means it's contained
    Directions dirs(Direction::north);
    EXPECT_TRUE(dirs);
    EXPECT_FALSE(dirs.empty());
    EXPECT_EQ(Direction::north, *dirs.begin());

    EXPECT_FALSE(dirs[Direction::none]);
    EXPECT_TRUE(dirs[Direction::north]);
}

TEST(Directions, None)
{   // None is a direction and can be represented
    Directions dirs(Direction::none);
    EXPECT_TRUE(dirs);
    EXPECT_FALSE(dirs.empty());
    EXPECT_EQ(Direction::none, *dirs.begin());

    EXPECT_TRUE(dirs[Direction::none]);
    EXPECT_FALSE(dirs[Direction::north]);
}

TEST(Directions, All)
{   // The Directions::all contains all directions except none
    EXPECT_TRUE(Directions::all);
    EXPECT_FALSE(Directions::all.empty());

    EXPECT_FALSE(Directions::all[Direction::none]);
    EXPECT_TRUE(Directions::all[Direction::north]);
    EXPECT_TRUE(Directions::all[Direction::down]);
}

TEST(Directions, Operation)
{   // The Directions::all contains all directions except none
    EXPECT_TRUE((Directions() | Direction::north)[Direction::north]);
    EXPECT_TRUE((Direction::north | Direction::south)[Direction::south] );

}
}}  // Test
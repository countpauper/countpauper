#include "Game/Orientation.h"
#include "Geometry/Position.h"
#include "Geometry/Geometry.h"
#include "Geometry/Angles.h"
#include <gtest/gtest.h>
#include <cmath>

namespace  Game::Test
{

TEST(Orientation, None)
{
    EXPECT_TRUE(Orientation().IsNone());
	EXPECT_TRUE(Orientation::none.IsNone());
    EXPECT_FALSE(Orientation::none.IsPosititve());
    EXPECT_FALSE(Orientation::none.IsNegative());
    EXPECT_TRUE(Orientation::none.IsValid());
    EXPECT_FALSE(Orientation::none.IsX());
    EXPECT_FALSE(Orientation::none.Vector());
}

TEST(Orientation, CardinalPositions)
{
    EXPECT_EQ(Orientation(Engine::Position(0, 0, 0)), Orientation::none);
    EXPECT_EQ(Orientation(Engine::Position(0, -1, 0)), Orientation::south);
    EXPECT_EQ(Orientation(Engine::Position(0, 1, 0)), Orientation::north);
    EXPECT_EQ(Orientation(Engine::Position(-1, 0, 0)), Orientation::west);
    EXPECT_EQ(Orientation(Engine::Position(1, 0, 0)), Orientation::east);
}


TEST(Orientation, DiagonalPositions)
{
	EXPECT_EQ(Orientation(Engine::Position(-1, -1, 0)), Orientation::west);
	EXPECT_EQ(Orientation(Engine::Position(1, -1, 0)), Orientation::south);
	EXPECT_EQ(Orientation(Engine::Position(1, 1, 0)), Orientation::east);
	EXPECT_EQ(Orientation(Engine::Position(-1, 1, 0)), Orientation::north);
}

TEST(Orientation, VerticalPositions)
{
	EXPECT_EQ(Orientation(0, 0, 1), Orientation::up);
	EXPECT_EQ(Orientation(0, 0,-1), Orientation::down);
	EXPECT_EQ(Orientation(1, 1,-2), Orientation::down);
	EXPECT_EQ(Orientation(1, 0, 1), Orientation::east);
}

TEST(Orientation, Axis)
{
    EXPECT_TRUE(Orientation::east.Axis().IsX());
    EXPECT_TRUE(Orientation::west.Axis().IsX());
    EXPECT_EQ(Orientation::east.Axis(), Orientation::west.Axis());
    EXPECT_TRUE(Orientation::north.Axis().IsY());
    EXPECT_TRUE(Orientation::south.Axis().IsY());
    EXPECT_EQ(Orientation::north.Axis(), Orientation::south.Axis());
    EXPECT_TRUE(Orientation::up.Axis().IsZ());
    EXPECT_TRUE(Orientation::down.Axis().IsZ());
    EXPECT_EQ(Orientation::up.Axis(), Orientation::down.Axis());
}

TEST(Orientation, Opposite)
{
    EXPECT_TRUE(Orientation::east.IsOpposite(Orientation::west));
    EXPECT_TRUE(Orientation::south.IsOpposite(Orientation::north));
    EXPECT_TRUE(Orientation::up.IsOpposite(Orientation::down));
	EXPECT_FALSE(Orientation::east.IsOpposite(Orientation::north));
	EXPECT_FALSE(Orientation::up.IsOpposite(Orientation::west));
	EXPECT_FALSE(Orientation::none.IsOpposite(Orientation::west));
	EXPECT_FALSE(Orientation::east.IsOpposite(Orientation::east));

	EXPECT_TRUE(Orientation::east.IsOpposite(Orientation::east.Opposite()));
	EXPECT_TRUE(Orientation::south.IsOpposite(Orientation::south.Opposite()));
	EXPECT_TRUE(Orientation::down.IsOpposite(Orientation::down.Opposite()));
}

TEST(Orientation, PositiveNegative)
{
    EXPECT_TRUE(Orientation::east.IsPosititve() ^ Orientation::west.IsPosititve());
    EXPECT_TRUE(Orientation::north.IsPosititve() ^ Orientation::south.IsPosititve());
    EXPECT_TRUE(Orientation::up.IsPosititve() ^ Orientation::down.IsPosititve());

    EXPECT_TRUE(Orientation::east.IsNegative() ^ Orientation::west.IsNegative());
    EXPECT_TRUE(Orientation::north.IsNegative() ^ Orientation::south.IsNegative());
    EXPECT_TRUE(Orientation::up.IsNegative() ^ Orientation::down.IsNegative());

    for (auto dir : Orientation::all)
    {
        EXPECT_TRUE(dir.IsPosititve() ^ dir.IsNegative()) << dir;
    }
}

TEST(Orientation, Perpendiclar)
{
    EXPECT_TRUE(Orientation::north.Perpendicular(Orientation::east).IsPerpendicular(Orientation::north));
    EXPECT_TRUE(Orientation::north.Perpendicular(Orientation::east).IsPerpendicular(Orientation::west));
    EXPECT_TRUE(Orientation::up.Perpendicular(Orientation::east).IsPerpendicular(Orientation::down));
    EXPECT_TRUE(Orientation::up.Perpendicular(Orientation::east).IsPerpendicular(Orientation::down));
    EXPECT_THROW(Orientation::up.Perpendicular(Orientation::down), std::exception);
    EXPECT_THROW(Orientation::none.Perpendicular(Orientation::west), std::exception);
    EXPECT_THROW(Orientation::north.Perpendicular(Orientation::none), std::exception);
}

TEST(Orientation, Angles)
{
	EXPECT_TRUE(std::isnan(Orientation::none.Angle()));
	EXPECT_EQ(Orientation::east.Angle(), 0);
	EXPECT_NEAR(Orientation::north.Angle(), 0.5*Engine::PI, 0.001);
	EXPECT_NEAR(Orientation::south.Angle(), -0.5*Engine::PI, 0.001);
	EXPECT_NEAR(Orientation::west.Angle(), Engine::PI, 0.001);
	EXPECT_TRUE(std::isnan(Orientation::up.Angle()));
	EXPECT_TRUE(std::isnan(Orientation::down.Angle()));
}

TEST(Orientation, Clockwise)
{
	EXPECT_TRUE(Orientation::east.IsClockwise(Orientation::north));
	EXPECT_TRUE(Orientation::south.IsClockwise(Orientation::east));
	EXPECT_TRUE(Orientation::west.IsClockwise(Orientation::south));
	EXPECT_TRUE(Orientation::north.IsClockwise(Orientation::west));

	EXPECT_FALSE(Orientation::west.IsClockwise(Orientation::north));
	EXPECT_FALSE(Orientation::north.IsClockwise(Orientation::east));
	EXPECT_FALSE(Orientation::east.IsClockwise(Orientation::south));
	EXPECT_FALSE(Orientation::south.IsClockwise(Orientation::west));

	EXPECT_FALSE(Orientation::east.IsClockwise(Orientation::west));
	EXPECT_FALSE(Orientation::south.IsClockwise(Orientation::north));
	EXPECT_FALSE(Orientation::none.IsClockwise(Orientation::west));
	EXPECT_FALSE(Orientation::up.IsClockwise(Orientation::down));
	EXPECT_FALSE(Orientation::down.IsClockwise(Orientation::north));
}


TEST(Orientation, CounterClockwise)
{
	EXPECT_TRUE(Orientation::west.IsCounterClockwise(Orientation::north));
	EXPECT_TRUE(Orientation::north.IsCounterClockwise(Orientation::east));
	EXPECT_TRUE(Orientation::east.IsCounterClockwise(Orientation::south));
	EXPECT_TRUE(Orientation::south.IsCounterClockwise(Orientation::west));

	EXPECT_FALSE(Orientation::east.IsCounterClockwise(Orientation::north));
	EXPECT_FALSE(Orientation::south.IsCounterClockwise(Orientation::east));
	EXPECT_FALSE(Orientation::west.IsCounterClockwise(Orientation::south));
	EXPECT_FALSE(Orientation::north.IsCounterClockwise(Orientation::west));

	EXPECT_FALSE(Orientation::east.IsCounterClockwise(Orientation::west));
	EXPECT_FALSE(Orientation::south.IsCounterClockwise(Orientation::north));
	EXPECT_FALSE(Orientation::none.IsCounterClockwise(Orientation::west));
	EXPECT_FALSE(Orientation::up.IsCounterClockwise(Orientation::down));
	EXPECT_FALSE(Orientation::down.IsCounterClockwise(Orientation::north));
}

TEST(Orientation, EvenDivision)
{
    // even division of each direction in a 5x5 map
    // 4 orientations x 6 places + 1 x none = 25 positions
    std::map<Orientation, unsigned> count =
    {
        { Orientation(Orientation::none), 0 },
        { Orientation(Orientation::north), 0 },
        { Orientation(Orientation::east), 0 },
        { Orientation(Orientation::south), 0 },
        { Orientation(Orientation::west), 0 }
    };

    for (int y = -2; y <= 2; ++y)
    {
        for (int x = -2; x <= 2; ++x)
        {
            Orientation dir(Engine::Position(x, y, 0));
            count[dir] += 1;
        }
    }
    EXPECT_TRUE(count[Orientation::none] == 1);
    EXPECT_TRUE(count[Orientation::north] == 6);
    EXPECT_TRUE(count[Orientation::east] == 6);
    EXPECT_TRUE(count[Orientation::south] == 6);
    EXPECT_TRUE(count[Orientation::west] == 6);
}

TEST(Orientation, Vector)
{

	for (auto dir : Orientation::all)
	{
		EXPECT_EQ(Orientation(dir.Vector()), dir);
	}

}

TEST(Orientation, Prone)
{
	EXPECT_TRUE(Orientation::up.IsProne());
	EXPECT_TRUE(Orientation::down.IsProne());
	EXPECT_FALSE(Orientation::east.IsProne());
	EXPECT_FALSE(Orientation::north.IsProne());
	EXPECT_FALSE(Orientation::none.IsProne());
}

TEST(Orientation, Turn)
{
	EXPECT_EQ(Orientation::east.Turn(Orientation::left), Orientation::north);
	EXPECT_EQ(Orientation::north.Turn(Orientation::backward), Orientation::south);
	EXPECT_EQ(Orientation::south.Turn(Orientation::right), Orientation::west);
	EXPECT_EQ(Orientation::west.Turn(Orientation::forward), Orientation::west);

	EXPECT_EQ(Orientation::east.Turn(Orientation::up), Orientation::up);
	EXPECT_EQ(Orientation::north.Turn(Orientation::down), Orientation::down);

	EXPECT_EQ(Orientation::down.Turn(Orientation::right), Orientation::down);
	EXPECT_EQ(Orientation::down.Turn(Orientation::up), Orientation::up);
}


TEST(Orientations, Empty)
{   // Default initialized orientations are empty
    Orientations dirs;
    EXPECT_FALSE(dirs);
    EXPECT_TRUE(dirs.empty());
    EXPECT_TRUE(dirs.begin() == dirs.end());
    EXPECT_FALSE(dirs[Orientation::none]);
    EXPECT_FALSE(dirs[Orientation::north]);
}

TEST(Orientations, Single)
{   // Initializing with one orientations means it's contained
    Orientations dirs(Orientation::north);
    EXPECT_TRUE(dirs);
    EXPECT_FALSE(dirs.empty());
    EXPECT_EQ(Orientation::north, *dirs.begin());

    EXPECT_FALSE(dirs[Orientation::none]);
    EXPECT_TRUE(dirs[Orientation::north]);
}

TEST(Orientations, None)
{   // None is a direction and can be represented
    Orientations dirs(Orientation::none);
    EXPECT_TRUE(dirs);
    EXPECT_FALSE(dirs.empty());
    EXPECT_EQ(Orientation::none, *dirs.begin());

    EXPECT_TRUE(dirs[Orientation::none]);
    EXPECT_FALSE(dirs[Orientation::north]);
}

TEST(Orientations, All)
{   // The Orientations::all contains all orientations except none
    EXPECT_TRUE(Orientations::all);
    EXPECT_FALSE(Orientations::all.empty());

    EXPECT_FALSE(Orientations::all[Orientation::none]);
    EXPECT_TRUE(Orientations::all[Orientation::north]);
    EXPECT_TRUE(Orientations::all[Orientation::down]);
}

TEST(Orientations, Operation)
{   // The Orientations::all contains all orientations except none
    EXPECT_TRUE((Orientations() | Orientation::north)[Orientation::north]);
    EXPECT_TRUE((Orientation::north | Orientation::south)[Orientation::south] );

}
}  // Game::Test

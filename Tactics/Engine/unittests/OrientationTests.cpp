#include "Geometry/Orientation.h"
#include "Geometry/Position.h"
#include "Geometry/Geometry.h"
#include "Geometry/Angles.h"
#include <gtest/gtest.h>
#include <cmath>

namespace  Engine::Test
{

TEST(Orientation, None)
{
    EXPECT_TRUE(Orientation().IsNone());
    EXPECT_FALSE(Orientation());
	EXPECT_TRUE(Orientation::none.IsNone());
    EXPECT_FALSE(Orientation::none.IsPosititve());
    EXPECT_FALSE(Orientation::none.IsNegative());
    EXPECT_FALSE(Orientation::none.IsX());
    EXPECT_FALSE(Orientation::none.GetVector());
}

TEST(Orientation, CardinalPositions)
{
    EXPECT_EQ(Orientation(Engine::Position(0, 0, 0)), Orientation::none);
    EXPECT_EQ(Orientation(Engine::Position(0, -1, 0)), Orientation::back);
    EXPECT_EQ(Orientation(Engine::Position(0, 1, 0)), Orientation::front);
    EXPECT_EQ(Orientation(Engine::Position(-1, 0, 0)), Orientation::left);
    EXPECT_EQ(Orientation(Engine::Position(1, 0, 0)), Orientation::right);
}


TEST(Orientation, DiagonalPositions)
{
	EXPECT_EQ(Orientation(Engine::Position(-2, -1, 0)), Orientation::left);
	EXPECT_EQ(Orientation(Engine::Position(1, -1, 0)), Orientation::right);
	EXPECT_EQ(Orientation(Engine::Position(2, 1, 0)), Orientation::right);
	EXPECT_EQ(Orientation(Engine::Position(-1, 1, 0)), Orientation::left);
}

TEST(Orientation, VerticalPositions)
{
	EXPECT_EQ(Orientation(0, 0, 1), Orientation::up);
	EXPECT_EQ(Orientation(0, 0,-1), Orientation::down);
	EXPECT_EQ(Orientation(1, 1,-2), Orientation::down);
	EXPECT_EQ(Orientation(2, 0, 1), Orientation::right);
}

TEST(Orientation, Axis)
{
    EXPECT_TRUE(Orientation::right.Axis().IsX());
    EXPECT_TRUE(Orientation::left.Axis().IsX());
    EXPECT_EQ(Orientation::right.Axis(), Orientation::left.Axis());
    EXPECT_TRUE(Orientation::front.Axis().IsY());
    EXPECT_TRUE(Orientation::back.Axis().IsY());
    EXPECT_EQ(Orientation::front.Axis(), Orientation::back.Axis());
    EXPECT_TRUE(Orientation::up.Axis().IsZ());
    EXPECT_TRUE(Orientation::down.Axis().IsZ());
    EXPECT_EQ(Orientation::up.Axis(), Orientation::down.Axis());
}

TEST(Orientation, Opposite)
{
    EXPECT_FALSE(Orientation::none.IsOpposite(Orientation::none));
    EXPECT_TRUE(Orientation::right.IsOpposite(Orientation::left));
    EXPECT_TRUE(Orientation::back.IsOpposite(Orientation::front));
    EXPECT_TRUE(Orientation::up.IsOpposite(Orientation::down));
	EXPECT_FALSE(Orientation::right.IsOpposite(Orientation::front));
	EXPECT_FALSE(Orientation::up.IsOpposite(Orientation::left));
	EXPECT_FALSE(Orientation::none.IsOpposite(Orientation::left));
	EXPECT_FALSE(Orientation::right.IsOpposite(Orientation::right));

	EXPECT_TRUE(Orientation::right.IsOpposite(Orientation::right.Opposite()));
	EXPECT_TRUE(Orientation::back.IsOpposite(Orientation::back.Opposite()));
	EXPECT_TRUE(Orientation::down.IsOpposite(Orientation::down.Opposite()));
}

TEST(Orientation, PositiveNegative)
{
    EXPECT_TRUE(Orientation::right.IsPosititve() ^ Orientation::left.IsPosititve());
    EXPECT_TRUE(Orientation::front.IsPosititve() ^ Orientation::back.IsPosititve());
    EXPECT_TRUE(Orientation::up.IsPosititve() ^ Orientation::down.IsPosititve());

    EXPECT_TRUE(Orientation::right.IsNegative() ^ Orientation::left.IsNegative());
    EXPECT_TRUE(Orientation::front.IsNegative() ^ Orientation::back.IsNegative());
    EXPECT_TRUE(Orientation::up.IsNegative() ^ Orientation::down.IsNegative());

    for (auto dir : Orientations::all)
    {
        EXPECT_TRUE(dir.IsPosititve() ^ dir.IsNegative()) << dir;
    }
}

TEST(Orientation, Perpendiclar)
{
    EXPECT_TRUE(Orientation::front.Perpendicular(Orientation::right).IsPerpendicular(Orientation::front));
    EXPECT_TRUE(Orientation::front.Perpendicular(Orientation::right).IsPerpendicular(Orientation::left));
    EXPECT_TRUE(Orientation::up.Perpendicular(Orientation::right).IsPerpendicular(Orientation::down));
    EXPECT_TRUE(Orientation::up.Perpendicular(Orientation::right).IsPerpendicular(Orientation::down));
    EXPECT_FALSE(Orientation::up.Perpendicular(Orientation::down));
    EXPECT_FALSE(Orientation::none.Perpendicular(Orientation::left));
    EXPECT_FALSE(Orientation::front.Perpendicular(Orientation::none));
}

TEST(Orientation, Angles)
{
	EXPECT_TRUE(std::isnan(Orientation::none.Angle()));
	EXPECT_EQ(Orientation::front.Angle(), 0);
	EXPECT_NEAR(Orientation::right.Angle(), -0.5*Engine::PI, 0.001);
	EXPECT_NEAR(Orientation::left.Angle(), 0.5*Engine::PI, 0.001);
	EXPECT_NEAR(Orientation::back.Angle(), Engine::PI, 0.001);
	EXPECT_TRUE(std::isnan(Orientation::up.Angle()));
	EXPECT_TRUE(std::isnan(Orientation::down.Angle()));
}

TEST(Orientation, Clockwise)
{
	EXPECT_TRUE(Orientation::left.IsClockwise(Orientation::front));
	EXPECT_TRUE(Orientation::front.IsClockwise(Orientation::right));
	EXPECT_TRUE(Orientation::right.IsClockwise(Orientation::back));
	EXPECT_TRUE(Orientation::back.IsClockwise(Orientation::left));

	EXPECT_FALSE(Orientation::right.IsClockwise(Orientation::front));
	EXPECT_FALSE(Orientation::back.IsClockwise(Orientation::right));
	EXPECT_FALSE(Orientation::left.IsClockwise(Orientation::back));
	EXPECT_FALSE(Orientation::front.IsClockwise(Orientation::left));

	EXPECT_FALSE(Orientation::right.IsClockwise(Orientation::left));
	EXPECT_FALSE(Orientation::back.IsClockwise(Orientation::front));
	EXPECT_FALSE(Orientation::none.IsClockwise(Orientation::left));
	EXPECT_FALSE(Orientation::up.IsClockwise(Orientation::down));
	EXPECT_FALSE(Orientation::down.IsClockwise(Orientation::front));
}

TEST(Orientation, CounterClockwise)
{
	EXPECT_TRUE(Orientation::right.IsCounterClockwise(Orientation::front));
	EXPECT_TRUE(Orientation::back.IsCounterClockwise(Orientation::right));
	EXPECT_TRUE(Orientation::left.IsCounterClockwise(Orientation::back));
	EXPECT_TRUE(Orientation::front.IsCounterClockwise(Orientation::left));

	EXPECT_FALSE(Orientation::left.IsCounterClockwise(Orientation::front));
	EXPECT_FALSE(Orientation::front.IsCounterClockwise(Orientation::right));
	EXPECT_FALSE(Orientation::right.IsCounterClockwise(Orientation::back));
	EXPECT_FALSE(Orientation::back.IsCounterClockwise(Orientation::left));

	EXPECT_FALSE(Orientation::right.IsCounterClockwise(Orientation::left));
	EXPECT_FALSE(Orientation::back.IsCounterClockwise(Orientation::front));
	EXPECT_FALSE(Orientation::none.IsCounterClockwise(Orientation::left));
	EXPECT_FALSE(Orientation::up.IsCounterClockwise(Orientation::down));
	EXPECT_FALSE(Orientation::down.IsCounterClockwise(Orientation::front));
}

TEST(Orientation, EvenDivision)
{
    // even division of each direction in a 5x5 map
    // 4 orientations x 6 places + 1 x none = 25 positions
    std::map<Orientation, unsigned> count =
    {
        { Orientation(Orientation::none), 0 },
        { Orientation(Orientation::front), 0 },
        { Orientation(Orientation::right), 0 },
        { Orientation(Orientation::back), 0 },
        { Orientation(Orientation::left), 0 }
    };

    for (int y = -2; y <= 2; ++y)
    {
        for (int x = -2; x <= 2; ++x)
        {
            Orientation dir(Engine::Position(x, y, 0));
            count[dir] += 1;
        }
    }
    EXPECT_EQ(count[Orientation::none], 1);
    EXPECT_EQ(count[Orientation::front], 6);
    EXPECT_EQ(count[Orientation::right], 6);
    EXPECT_EQ(count[Orientation::back], 6);
    EXPECT_EQ(count[Orientation::left], 6);
}

TEST(Orientation, Vector)
{
	for (auto ori : Orientations::all)
	{
        auto v = ori.GetVector();
        auto newori = Orientation(v);
        EXPECT_EQ(ori, newori);
	}

}

TEST(Orientation, Index)
{
    EXPECT_EQ(Orientation().Index(), -1);
    int expectedIndex = 0;
	for (auto ori : Orientations::all)
	{
        EXPECT_EQ(ori.Index(), expectedIndex) << ori;
        ++expectedIndex;
    }
}

TEST(Orientation, Turn)
{
	EXPECT_EQ(Orientation::right.Turn(Orientation::left), Orientation::front);
	EXPECT_EQ(Orientation::front.Turn(Orientation::back), Orientation::back);
	EXPECT_EQ(Orientation::back.Turn(Orientation::right), Orientation::left);
	EXPECT_EQ(Orientation::left.Turn(Orientation::front), Orientation::left);

	EXPECT_EQ(Orientation::right.Turn(Orientation::up), Orientation::up);
	EXPECT_EQ(Orientation::front.Turn(Orientation::down), Orientation::down);

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
    EXPECT_FALSE(dirs[Orientation::front]);
}

TEST(Orientations, Single)
{   // Initializing with one orientations means it's contained
    Orientations dirs(Orientation::front);
    EXPECT_TRUE(dirs);
    EXPECT_FALSE(dirs.empty());
    EXPECT_EQ(Orientation::front, *dirs.begin());

    EXPECT_FALSE(dirs[Orientation::none]);
    EXPECT_TRUE(dirs[Orientation::front]);
}

TEST(Orientations, None)
{   // None is a direction and can be represented
    Orientations dirs(Orientation::none);
    EXPECT_TRUE(dirs);
    EXPECT_FALSE(dirs.empty());
    EXPECT_EQ(Orientation::none, *dirs.begin());

    EXPECT_TRUE(dirs[Orientation::none]);
    EXPECT_FALSE(dirs[Orientation::front]);
}

TEST(Orientations, All)
{   // The Orientations::all contains all orientations except none
    EXPECT_TRUE(Orientations::all);
    EXPECT_FALSE(Orientations::all.empty());

    EXPECT_FALSE(Orientations::all[Orientation::none]);
    EXPECT_TRUE(Orientations::all[Orientation::front]);
    EXPECT_TRUE(Orientations::all[Orientation::down]);
}

TEST(Orientations, Operation)
{   // The Orientations::all contains all orientations except none
    EXPECT_TRUE((Orientations() | Orientation::front)[Orientation::front]);
    EXPECT_TRUE((Orientation::front | Orientation::back)[Orientation::back] );

}
}  // Game::Test

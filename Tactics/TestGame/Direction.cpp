#include <boost/test/unit_test.hpp>
#include "Game/Direction.h"
#include "Game/Position.h"
#include "Engine/Geometry.h"

namespace Game
{
namespace Test
{

BOOST_AUTO_TEST_SUITE(DirectionTest);

BOOST_AUTO_TEST_CASE(None)
{
	BOOST_CHECK(Direction().IsNone());
	BOOST_CHECK(Direction::none.IsNone());
}

BOOST_AUTO_TEST_CASE(CardinalPositions)
{
    BOOST_CHECK_EQUAL(Direction(Position(0, 0)), Direction::none);
    BOOST_CHECK_EQUAL(Direction(Position(0, -1)), Direction::south);
    BOOST_CHECK_EQUAL(Direction(Position(0, 1)), Direction::north);
    BOOST_CHECK_EQUAL(Direction(Position(-1, 0)), Direction::west);
    BOOST_CHECK_EQUAL(Direction(Position(1, 0)), Direction::east);
}


BOOST_AUTO_TEST_CASE(DiagonalPositions)
{
	BOOST_CHECK_EQUAL(Direction(Position(-1, -1)), Direction::west);
	BOOST_CHECK_EQUAL(Direction(Position(1, -1)), Direction::south);
	BOOST_CHECK_EQUAL(Direction(Position(1, 1)), Direction::east);
	BOOST_CHECK_EQUAL(Direction(Position(-1, 1)), Direction::north);
}

BOOST_AUTO_TEST_CASE(VerticalPositions)
{
	BOOST_CHECK_EQUAL(Direction(0, 0, 1), Direction::up);
	BOOST_CHECK_EQUAL(Direction(0, 0,-1), Direction::down);
	BOOST_CHECK_EQUAL(Direction(1, 1,-2), Direction::down);
	BOOST_CHECK_EQUAL(Direction(1, 0, 1), Direction::east);
}


BOOST_AUTO_TEST_CASE(Opposite)
{
	BOOST_CHECK(Direction::east.IsOpposite(Direction::west));
	BOOST_CHECK(Direction::south.IsOpposite(Direction::north));
	BOOST_CHECK(Direction::up.IsOpposite(Direction::down));
	BOOST_CHECK(!Direction::east.IsOpposite(Direction::north));
	BOOST_CHECK(!Direction::up.IsOpposite(Direction::west));
	BOOST_CHECK(!Direction::none.IsOpposite(Direction::west));
	BOOST_CHECK(!Direction::east.IsOpposite(Direction::east));

	BOOST_CHECK(Direction::east.IsOpposite(Direction::east.Opposite()));
	BOOST_CHECK(Direction::south.IsOpposite(Direction::south.Opposite()));
	BOOST_CHECK(Direction::down.IsOpposite(Direction::down.Opposite()));
}

BOOST_AUTO_TEST_CASE(Angles)
{
	BOOST_CHECK(std::isnan(Direction::none.Angle()));
	BOOST_CHECK_EQUAL(Direction::east.Angle(), 0);
	BOOST_CHECK_CLOSE(Direction::north.Angle(), 0.5*Engine::PI, 0.001);
	BOOST_CHECK_CLOSE(Direction::south.Angle(), -0.5*Engine::PI, 0.001);
	BOOST_CHECK_CLOSE(Direction::west.Angle(), Engine::PI, 0.001);
	BOOST_CHECK(std::isnan(Direction::up.Angle()));
	BOOST_CHECK(std::isnan(Direction::down.Angle()));
}

BOOST_AUTO_TEST_CASE(Clockwise)
{
	BOOST_CHECK(Direction::east.IsClockwise(Direction::north));
	BOOST_CHECK(Direction::south.IsClockwise(Direction::east));
	BOOST_CHECK(Direction::west.IsClockwise(Direction::south));
	BOOST_CHECK(Direction::north.IsClockwise(Direction::west));

	BOOST_CHECK(!Direction::west.IsClockwise(Direction::north));
	BOOST_CHECK(!Direction::north.IsClockwise(Direction::east));
	BOOST_CHECK(!Direction::east.IsClockwise(Direction::south));
	BOOST_CHECK(!Direction::south.IsClockwise(Direction::west));

	BOOST_CHECK(!Direction::east.IsClockwise(Direction::west));
	BOOST_CHECK(!Direction::south.IsClockwise(Direction::north));
	BOOST_CHECK(!Direction::none.IsClockwise(Direction::west));
	BOOST_CHECK(!Direction::up.IsClockwise(Direction::down));
	BOOST_CHECK(!Direction::down.IsClockwise(Direction::north));
}


BOOST_AUTO_TEST_CASE(CounterClockwise)
{
	BOOST_CHECK(Direction::west.IsCounterClockwise(Direction::north));
	BOOST_CHECK(Direction::north.IsCounterClockwise(Direction::east));
	BOOST_CHECK(Direction::east.IsCounterClockwise(Direction::south));
	BOOST_CHECK(Direction::south.IsCounterClockwise(Direction::west));

	BOOST_CHECK(!Direction::east.IsCounterClockwise(Direction::north));
	BOOST_CHECK(!Direction::south.IsCounterClockwise(Direction::east));
	BOOST_CHECK(!Direction::west.IsCounterClockwise(Direction::south));
	BOOST_CHECK(!Direction::north.IsCounterClockwise(Direction::west));

	BOOST_CHECK(!Direction::east.IsCounterClockwise(Direction::west));
	BOOST_CHECK(!Direction::south.IsCounterClockwise(Direction::north));
	BOOST_CHECK(!Direction::none.IsCounterClockwise(Direction::west));
	BOOST_CHECK(!Direction::up.IsCounterClockwise(Direction::down));
	BOOST_CHECK(!Direction::down.IsCounterClockwise(Direction::north));
}

BOOST_AUTO_TEST_CASE(EvenDivision)
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
            Direction dir(Position(x, y));
            count[dir] += 1;
        }
    }
    BOOST_CHECK(count[Direction::none] == 1);
    BOOST_CHECK(count[Direction::north] == 6);
    BOOST_CHECK(count[Direction::east] == 6);
    BOOST_CHECK(count[Direction::south] == 6);
    BOOST_CHECK(count[Direction::west] == 6);
}

BOOST_AUTO_TEST_CASE(Vector)
{
	std::vector<Direction> allHorizontalDirections = {
		Direction::none,
		Direction::north,
		Direction::east,
		Direction::south,
		Direction::west
	};
	for (auto dir : allHorizontalDirections)
	{
		BOOST_CHECK_EQUAL(Direction(dir.Vector()), dir);
	}
	BOOST_CHECK(!Direction::up.Vector());
	BOOST_CHECK(!Direction::down.Vector());

}

BOOST_AUTO_TEST_CASE(Prone)
{
	BOOST_CHECK(Direction::up.IsProne());
	BOOST_CHECK(Direction::down.IsProne());
	BOOST_CHECK(!Direction::east.IsProne());
	BOOST_CHECK(!Direction::north.IsProne());
	BOOST_CHECK(!Direction::none.IsProne());
}

BOOST_AUTO_TEST_CASE(Turn)
{
	BOOST_CHECK_EQUAL(Direction::east.Turn(Direction::left), Direction::north);
	BOOST_CHECK_EQUAL(Direction::north.Turn(Direction::backward), Direction::south);
	BOOST_CHECK_EQUAL(Direction::south.Turn(Direction::right), Direction::west);
	BOOST_CHECK_EQUAL(Direction::west.Turn(Direction::forward), Direction::west);

	BOOST_CHECK_EQUAL(Direction::east.Turn(Direction::up), Direction::up);
	BOOST_CHECK_EQUAL(Direction::north.Turn(Direction::down), Direction::down);

	BOOST_CHECK_EQUAL(Direction::down.Turn(Direction::right), Direction::down);
	BOOST_CHECK_EQUAL(Direction::down.Turn(Direction::up), Direction::up);
}

BOOST_AUTO_TEST_SUITE_END()
}}  // Test
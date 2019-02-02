#include <boost/test/unit_test.hpp>
#include "Game/Direction.h"
#include "Game/Position.h"

namespace Game
{
namespace Test
{

BOOST_AUTO_TEST_SUITE(DirectionTest);

BOOST_AUTO_TEST_CASE(None)
{
    BOOST_CHECK(Direction(Direction::None).IsNone());
}

BOOST_AUTO_TEST_CASE(CardinalPositions)
{
    BOOST_CHECK_EQUAL(Direction(Position(0, 0)), Direction::None);
    BOOST_CHECK_EQUAL(Direction(Position(0, -1)), Direction::North);
    BOOST_CHECK_EQUAL(Direction(Position(0, 1)), Direction::South);
    BOOST_CHECK_EQUAL(Direction(Position(-1, 0)), Direction::West);
    BOOST_CHECK_EQUAL(Direction(Position(1, 0)), Direction::East);
}

BOOST_AUTO_TEST_CASE(DiagonalPositions)
{
    BOOST_CHECK_EQUAL(Direction(Position(-1, -1)), Direction::West);
    BOOST_CHECK_EQUAL(Direction(Position(1, -1)), Direction::North);
    BOOST_CHECK_EQUAL(Direction(Position(1, 1)), Direction::East);
    BOOST_CHECK_EQUAL(Direction(Position(-1, 1)), Direction::South);
}

BOOST_AUTO_TEST_CASE(EvenDivision)
{
    // even division of each direction in a 5x5 map
    // 4 directions x 6 places + 1 x none = 25 positions
    std::map<Direction, unsigned> count =
    {
        { Direction(Direction::None), 0 },
        { Direction(Direction::North), 0 },
        { Direction(Direction::East), 0 },
        { Direction(Direction::South), 0 },
        { Direction(Direction::West), 0 }
    };

    for (int y = -2; y <= 2; ++y)
    {
        for (int x = -2; x <= 2; ++x)
        {
            Direction dir(Position(x, y));
            count[dir] += 1;
        }
    }
    BOOST_CHECK(count[Direction::None] == 1);
    BOOST_CHECK(count[Direction::North] == 6);
    BOOST_CHECK(count[Direction::East] == 6);
    BOOST_CHECK(count[Direction::South] == 6);
    BOOST_CHECK(count[Direction::West] == 6);
}

BOOST_AUTO_TEST_CASE(Vector)
{
    std::vector<Direction> allDirections = {
        Direction::None,
        Direction::North,
        Direction::East,
        Direction::South,
        Direction::West,
    };
    for (auto dir : allDirections)
    {
        BOOST_CHECK_EQUAL(Direction(dir.Vector()), dir);
    }
}
BOOST_AUTO_TEST_SUITE_END()
}}  // Test
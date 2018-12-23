#include <boost/test/unit_test.hpp>
#include "Direction.h"
#include "Position.h"


BOOST_AUTO_TEST_SUITE(Direction);

BOOST_AUTO_TEST_CASE(None)
{
    BOOST_CHECK(Game::Direction(Game::Direction::None).IsNone());
}

BOOST_AUTO_TEST_CASE(CardinalPositions)
{
    BOOST_CHECK_EQUAL(Game::Direction(Game::Position(0, 0)), Game::Direction::None);
    BOOST_CHECK_EQUAL(Game::Direction(Game::Position(0, -1)), Game::Direction::North);
    BOOST_CHECK_EQUAL(Game::Direction(Game::Position(0, 1)), Game::Direction::South);
    BOOST_CHECK_EQUAL(Game::Direction(Game::Position(-1, 0)), Game::Direction::West);
    BOOST_CHECK_EQUAL(Game::Direction(Game::Position(1, 0)), Game::Direction::East);
}

BOOST_AUTO_TEST_CASE(DiagonalPositions)
{
    BOOST_CHECK_EQUAL(Game::Direction(Game::Position(-1, -1)), Game::Direction::West);
    BOOST_CHECK_EQUAL(Game::Direction(Game::Position(1, -1)), Game::Direction::North);
    BOOST_CHECK_EQUAL(Game::Direction(Game::Position(1, 1)), Game::Direction::East);
    BOOST_CHECK_EQUAL(Game::Direction(Game::Position(-1, 1)), Game::Direction::South);
}

BOOST_AUTO_TEST_CASE(EvenDivision)
{
    // even division of each direction in a 5x5 map
    // 4 directions x 6 places + 1 x none = 25 positions
    std::map<Game::Direction, unsigned> count =
    {
        { Game::Direction(Game::Direction::None), 0 },
        { Game::Direction(Game::Direction::North), 0 },
        { Game::Direction(Game::Direction::East), 0 },
        { Game::Direction(Game::Direction::South), 0 },
        { Game::Direction(Game::Direction::West), 0 }
    };

    for (int y = -2; y <= 2; ++y)
    {
        for (int x = -2; x <= 2; ++x)
        {
            Game::Direction dir(Game::Position(x, y));
            count[dir] += 1;
        }
    }
    BOOST_CHECK(count[Game::Direction::None] == 1);
    BOOST_CHECK(count[Game::Direction::North] == 6);
    BOOST_CHECK(count[Game::Direction::East] == 6);
    BOOST_CHECK(count[Game::Direction::South] == 6);
    BOOST_CHECK(count[Game::Direction::West] == 6);
}

BOOST_AUTO_TEST_CASE(Vector)
{
    std::vector<Game::Direction> allDirections = {
        Game::Direction::None,
        Game::Direction::North,
        Game::Direction::East,
        Game::Direction::South,
        Game::Direction::West,
    };
    for (auto dir : allDirections)
    {
        BOOST_CHECK_EQUAL(Game::Direction(dir.Vector()), dir);
    }
}
BOOST_AUTO_TEST_SUITE_END()
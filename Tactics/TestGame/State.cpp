#include <boost/test/unit_test.hpp>
#include "Game/State.h"
#include "Data.h"

namespace Game
{
namespace Test
{
BOOST_AUTO_TEST_SUITE(StateTest);


BOOST_AUTO_TEST_CASE(BodyStatistics)
{
    Data::Simple b;
    State s(b);
    BOOST_CHECK_EQUAL(s.Strength(), b.Strength());
    BOOST_CHECK_EQUAL(s.Agility(), b.Agility());
    BOOST_CHECK_EQUAL(s.Constitution(), b.Constitution());
    BOOST_CHECK_EQUAL(s.Intelligence(), b.Intelligence());
    BOOST_CHECK_EQUAL(s.Wisdom(), b.Wisdom());

    BOOST_CHECK_EQUAL(s.AttributeScore(Attribute::Wisdom), b.Wisdom());
}




BOOST_AUTO_TEST_SUITE_END()
}
}  // Game::Test

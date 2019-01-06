#include <boost/test/unit_test.hpp>
#include "Anatomy.h"

BOOST_AUTO_TEST_SUITE(Anatomy);

BOOST_AUTO_TEST_CASE(ContainsPlane)
{
    Game::Anatomy a(Game::Plane::All, 0);
    BOOST_CHECK(a.Contains(Game::Anatomy(Game::Plane::Front, 0)));
    BOOST_CHECK(a.Contains(Game::Anatomy(Game::Plane::Back, 0)));
    BOOST_CHECK(a.Contains(Game::Anatomy(Game::Plane::Left, 0)));
    BOOST_CHECK(a.Contains(Game::Anatomy(Game::Plane::Right, 0)));
    BOOST_CHECK(a.Contains(Game::Anatomy(Game::Plane::Top, 0)));
    BOOST_CHECK(a.Contains(Game::Anatomy(Game::Plane::Bottom, 0)));
}


BOOST_AUTO_TEST_CASE(ContainsHeight)
{
    Game::Anatomy a(Game::Plane::Front, 1, 2);
    BOOST_CHECK(!a.Contains(Game::Anatomy(Game::Plane::Front, 0)));
    BOOST_CHECK(a.Contains(Game::Anatomy(Game::Plane::Front, 1)));
    BOOST_CHECK(a.Contains(Game::Anatomy(Game::Plane::Front, 2)));
    BOOST_CHECK(!a.Contains(Game::Anatomy(Game::Plane::Front, 3)));
    BOOST_CHECK(a.Contains(Game::Anatomy(Game::Plane::Front, 0, 2)));
    BOOST_CHECK(!a.Contains(Game::Anatomy(Game::Plane::Front, 3,2)));
}

BOOST_AUTO_TEST_CASE(Serialize)
{
    std::wstringstream stream(L"Front[1] Around[2-3]");
    Game::Anatomy a, b;
    stream >> a >> b;
    BOOST_CHECK_EQUAL(a.plane, Game::Plane::Front);
    BOOST_CHECK_EQUAL(a.position, 1);
    BOOST_CHECK_EQUAL(a.size, 1);
    BOOST_CHECK_EQUAL(b.plane, Game::Plane::Around);
    BOOST_CHECK_EQUAL(b.position, 2);
    BOOST_CHECK_EQUAL(b.size, 2);
}

BOOST_AUTO_TEST_CASE(SerializeFail)
{
    std::wstringstream stream(L"Front Back][ Left[ Right] None[]");
    Game::Anatomy f,b,l,r,n;
    BOOST_CHECK_THROW(stream >> f, std::exception);
    BOOST_CHECK_THROW(stream >> b, std::exception);
    BOOST_CHECK_THROW(stream >> l, std::exception);
    BOOST_CHECK_THROW(stream >> r, std::exception);
    BOOST_CHECK_THROW(stream >> n, std::exception);
}

BOOST_AUTO_TEST_SUITE_END()
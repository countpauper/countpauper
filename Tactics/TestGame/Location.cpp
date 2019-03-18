#include <boost/test/unit_test.hpp>
#include "Game/Location.h"

namespace Game
{
namespace Test
{

BOOST_AUTO_TEST_SUITE(LocationTest);


BOOST_AUTO_TEST_CASE(Null)
{
    Location a;
    BOOST_CHECK(!a);
}

BOOST_AUTO_TEST_CASE(ContainsPlane)
{
    Location a(Plane::All, 0);
    BOOST_CHECK(a.Contains(Location(Plane::Front, 0)));
    BOOST_CHECK(a.Contains(Location(Plane::Back, 0)));
    BOOST_CHECK(a.Contains(Location(Plane::Left, 0)));
    BOOST_CHECK(a.Contains(Location(Plane::Right, 0)));
    BOOST_CHECK(a.Contains(Location(Plane::Top, 0)));
    BOOST_CHECK(a.Contains(Location(Plane::Bottom, 0)));
}


BOOST_AUTO_TEST_CASE(ContainsHeight)
{
    Location a(Plane::Front, 1, 2);
    BOOST_CHECK(!a.Contains(Location(Plane::Front, 0)));
    BOOST_CHECK(a.Contains(Location(Plane::Front, 1)));
    BOOST_CHECK(a.Contains(Location(Plane::Front, 2)));
    BOOST_CHECK(!a.Contains(Location(Plane::Front, 3)));
    BOOST_CHECK(a.Contains(Location(Plane::Front, 0, 2)));
    BOOST_CHECK(!a.Contains(Location(Plane::Front, 3, 2)));
}

BOOST_AUTO_TEST_CASE(Serialize)
{
    std::wstringstream stream(L"Front[1] Around[2-3]");
    Location a, b;
    stream >> a >> b;
    BOOST_CHECK_EQUAL(a.plane, Plane::Front);
    BOOST_CHECK_EQUAL(a.position, 1);
    BOOST_CHECK_EQUAL(a.size, 1);
    BOOST_CHECK_EQUAL(b.plane, Plane::Around);
    BOOST_CHECK_EQUAL(b.position, 2);
    BOOST_CHECK_EQUAL(b.size, 2);
}

BOOST_AUTO_TEST_CASE(SerializeFail)
{
    std::wstringstream stream(L"Front Back][ Left[ Right] None[]");
    Location f, b, l, r, n;
    BOOST_CHECK_THROW(stream >> f, std::exception);
    BOOST_CHECK_THROW(stream >> b, std::exception);
    BOOST_CHECK_THROW(stream >> l, std::exception);
    BOOST_CHECK_THROW(stream >> r, std::exception);
    BOOST_CHECK_THROW(stream >> n, std::exception);
}

BOOST_AUTO_TEST_SUITE_END()

}
}
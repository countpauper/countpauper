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
    Location a(Plane::all, 0);
    BOOST_CHECK(a.Contains(Location(Plane::front, 0)));
    BOOST_CHECK(a.Contains(Location(Plane::back, 0)));
    BOOST_CHECK(a.Contains(Location(Plane::left, 0)));
    BOOST_CHECK(a.Contains(Location(Plane::right, 0)));
	BOOST_CHECK(a.Contains(Location(Plane::top, 0)));
	BOOST_CHECK(a.Contains(Location(Plane::bottom, 0)));
	BOOST_CHECK(a.Contains(Location(Plane::sagittal, 0)));
	BOOST_CHECK(a.Contains(Location(Plane::coronal, 0)));
	BOOST_CHECK(a.Contains(Location(Plane::transversal, 0)));
	Location b(Plane::sagittal, 1);
	BOOST_CHECK(b.Contains(Location(Plane::front, 1)));
	BOOST_CHECK(b.Contains(Location(Plane::back, 1)));
	BOOST_CHECK(b.Contains(Location(Plane::around, 1)));
	BOOST_CHECK(!b.Contains(Location(Plane::left, 1)));
	BOOST_CHECK(!b.Contains(Location(Plane::right, 1)));
	BOOST_CHECK(!b.Contains(Location(Plane::bottom, 1)));
	BOOST_CHECK(!b.Contains(Location(Plane::top, 1)));
	BOOST_CHECK(!b.Contains(Location(Plane::coronal, 1)));
	BOOST_CHECK(!b.Contains(Location(Plane::transversal, 1)));
}


BOOST_AUTO_TEST_CASE(ContainsHeight)
{
    Location a(Plane::front, 1, 2);
    BOOST_CHECK(!a.Contains(Location(Plane::front, 0)));
    BOOST_CHECK(a.Contains(Location(Plane::front, 1)));
    BOOST_CHECK(a.Contains(Location(Plane::front, 2)));
    BOOST_CHECK(!a.Contains(Location(Plane::front, 3)));
    BOOST_CHECK(a.Contains(Location(Plane::front, 0, 2)));
    BOOST_CHECK(!a.Contains(Location(Plane::front, 3, 2)));
}

BOOST_AUTO_TEST_CASE(Serialize)
{
    std::wstringstream stream(L"Front[1] Around[2-3]");
    Location a, b;
    stream >> a >> b;
    BOOST_CHECK_EQUAL(a.plane, Plane::front);
    BOOST_CHECK_EQUAL(a.position, 1);
    BOOST_CHECK_EQUAL(a.size, 1);
    BOOST_CHECK_EQUAL(b.plane, Plane::around);
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
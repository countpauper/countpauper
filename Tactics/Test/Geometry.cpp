#include <boost/test/unit_test.hpp>

#include "Geometry.h"
#include "Coordinate.h"

BOOST_AUTO_TEST_SUITE(Geometry);

BOOST_AUTO_TEST_CASE(Vector)
{
    Engine::Vector null(0, 0, 0);
    BOOST_CHECK_EQUAL(null.Length(), 0);
    BOOST_CHECK_EQUAL(null.SqrLength(), 0);
    BOOST_CHECK(!null);
    
    Engine::Vector v(1, 0, 0);
    BOOST_CHECK_EQUAL(v.Length(), 1);
    BOOST_CHECK_EQUAL(v.SqrLength(), 1);
    BOOST_CHECK(v);
}

BOOST_AUTO_TEST_CASE(Coordinate)
{
    Engine::Coordinate a(1, 2, 3);
    Engine::Coordinate b(4, 3, 3);

    Engine::Vector v = b - a;
    BOOST_CHECK_EQUAL(v, Engine::Vector(3, 1, 0));
}

const double maxError = 0.001;

BOOST_AUTO_TEST_CASE(Degrees)
{
    BOOST_CHECK_EQUAL(Engine::Rad2Deg(0), 0);
    BOOST_CHECK_CLOSE(Engine::Rad2Deg(Engine::PI), 180, maxError);
    BOOST_CHECK_CLOSE(Engine::Rad2Deg(-Engine::PI), -180, maxError);
    BOOST_CHECK_CLOSE(Engine::Rad2Deg(2 * Engine::PI), 360, maxError);
}


BOOST_AUTO_TEST_CASE(Turn)
{
    double halfpi = Engine::PI * 0.5;
    BOOST_CHECK_CLOSE(Engine::ShortestTurn(0, halfpi), halfpi, maxError);
    BOOST_CHECK_CLOSE(Engine::ShortestTurn(0, halfpi), halfpi, maxError);
    BOOST_CHECK_CLOSE(Engine::ShortestTurn(halfpi, 0), -halfpi, maxError);
    BOOST_CHECK_CLOSE(Engine::ShortestTurn(halfpi, Engine::PI), halfpi, maxError);
    BOOST_CHECK_CLOSE(Engine::ShortestTurn(0.75*Engine::PI, -0.75*Engine::PI), halfpi, maxError);
    BOOST_CHECK_CLOSE(Engine::ShortestTurn(-0.75*Engine::PI, 0.75*Engine::PI), -halfpi, maxError);
}




BOOST_AUTO_TEST_SUITE_END()
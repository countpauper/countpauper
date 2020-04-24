#include "pch.h"
#include "Geometry.h"
#include "Coordinate.h"
#include "Vector.h"
#include "Line.h"

namespace Engine::Test
{

TEST(Geometry, Vector)
{
    Engine::Vector null(0, 0, 0);
    EXPECT_EQ(null.Length(), 0);
    EXPECT_EQ(null.LengthSquared(), 0);
    EXPECT_FALSE(null);

    Engine::Vector v(1, 0, 0);
    EXPECT_EQ(v.Length(), 1);
    EXPECT_EQ(v.LengthSquared(), 1);
    EXPECT_TRUE(v);

    Engine::Vector v123(1, -2, 3);
    EXPECT_EQ(Vector() - v123, -v123);

    EXPECT_EQ(Vector(2, -4, 0), Vector(1, -2, 0) * 2.0);
    EXPECT_EQ(Vector(1, -2, 0), Vector(2, -4, 0) / 2.0);
}

TEST(Geometry, Coordinate)
{
    Engine::Coordinate a(1, 2, 3);
    Engine::Coordinate b(4, 3, 3);

    Engine::Vector v = b - a;
    EXPECT_EQ(v, Engine::Vector(3, 1, 0));
}

const double maxError = 0.001;

TEST(Geometry, Degrees)
{
    EXPECT_EQ(Engine::Rad2Deg(0), 0);
    EXPECT_NEAR(Engine::Rad2Deg(Engine::PI), 180, maxError);
    EXPECT_NEAR(Engine::Rad2Deg(-Engine::PI), -180, maxError);
    EXPECT_NEAR(Engine::Rad2Deg(2 * Engine::PI), 360, maxError);
}


TEST(Geometry, Turn)
{
    double halfpi = Engine::PI * 0.5;
    EXPECT_NEAR(Engine::ShortestTurn(0, halfpi), halfpi, maxError);
    EXPECT_NEAR(Engine::ShortestTurn(0, halfpi), halfpi, maxError);
    EXPECT_NEAR(Engine::ShortestTurn(halfpi, 0), -halfpi, maxError);
    EXPECT_NEAR(Engine::ShortestTurn(halfpi, Engine::PI), halfpi, maxError);
    EXPECT_NEAR(Engine::ShortestTurn(0.75*Engine::PI, -0.75*Engine::PI), halfpi, maxError);
    EXPECT_NEAR(Engine::ShortestTurn(-0.75*Engine::PI, 0.75*Engine::PI), -halfpi, maxError);
}

TEST(Line, Length)
{
    Engine::Coordinate a(1, 2, 3);
    Engine::Coordinate b(3, 0, 4);
    EXPECT_EQ(3.0, Line( a, b ).Length());
}

TEST(Line, Distance)
{
    Engine::Line l(Engine::Coordinate(1, 0, 0), Engine::Coordinate(3, 2, 0));
    EXPECT_EQ(1.0, Distance(Engine::Coordinate(1, -1, 0), l));
    EXPECT_EQ(2.0, Distance(Engine::Coordinate(5, 2, 0), l));
    EXPECT_EQ(3.0, Distance(Engine::Coordinate(0, 3, 1), l));

    EXPECT_EQ(1.0, Distance(Engine::Coordinate(1, 0, 0), Line(Engine::Coordinate(1, 1, 0), Engine::Coordinate(1, 1, 0))));
}
}
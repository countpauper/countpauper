#include <gtest/gtest.h>
#include "Geometry/Coordinate.h"
#include "Geometry/Vector.h"
#include "GTestGeometry.h"
#include <numeric>

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

    EXPECT_EQ(Vector::Z, Vector::X.Cross(Vector::Y));
    EXPECT_GT(0.0, Vector(1, 1, 0).Cross(Vector(1, -1, 0)).z);
    EXPECT_EQ(Vector(0, 0, -0.5), Vector(0.5, 0, 0).Cross(Vector(0, -1, 0)));
}


TEST(Geometry, Coordinate)
{
    Coordinate a(1, 2, 3);
    Coordinate b(4, 3, 3);

    Engine::Vector v = b - a;
    EXPECT_EQ(v, Engine::Vector(3, 1, 0));
}

}

#include "pch.h"
#include "Geometry.h"
#include "Coordinate.h"
#include "Vector.h"
#include "Line.h"
#include "Plane.h"
#include "Triangle.h"
#include "GTestGeometry.h"

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

    EXPECT_EQ(Vector(0, 0, 1), Vector(1, 0, 0).Cross(Vector(0, 1, 0)));
    EXPECT_GT(0.0, Vector(1, 1, 0).Cross(Vector(1, -1, 0)).z);
    EXPECT_EQ(Vector(0, 0, -0.5), Vector(0.5, 0, 0).Cross(Vector(0, -1, 0)));
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
    Engine::Line l0(Engine::Coordinate(1, 0, 0), Engine::Coordinate(1, 0, 0));
    EXPECT_EQ(1.0, l0.Distance(Engine::Coordinate(1, -1, 0)));

    Engine::Line l1(Engine::Coordinate(1, 0, 0), Engine::Coordinate(3, 2, 0));
    EXPECT_EQ(1.0, l1.Distance(Engine::Coordinate(1, -1, 0)));
    EXPECT_EQ(2.0, l1.Distance(Engine::Coordinate(5, 2, 0)));
    EXPECT_EQ(3.0, l1.Distance(Engine::Coordinate(0, 3, 1)));

    Line l2(Engine::Coordinate(1, 1, 0), Engine::Coordinate(1, 1, 0));
    EXPECT_EQ(1.0, l2.Distance(Engine::Coordinate(1, 0, 0)));
}

TEST(Plane, Null)
{
    EXPECT_FALSE(Plane(Vector(0, 0, 0), 1));
    EXPECT_FALSE(Plane(Vector(0, 0, 0), 1));
    EXPECT_FALSE(Plane(Coordinate::zero, Coordinate::zero, Coordinate::zero));
    EXPECT_FALSE(Plane(Coordinate(1, 0, 0), Coordinate(1, 0, 0), Coordinate(1, 0, 0)));
    EXPECT_EQ(Plane(Vector(0,0,0),0) , Plane(Coordinate(1, 0, 0), Coordinate(1, 0, 0), Coordinate(1, 0, 0)));
    EXPECT_FALSE(Plane::null);
    EXPECT_FALSE(Plane::null.Normalized());
}

TEST(Plane, XY)
{
    EXPECT_TRUE(Plane::xy);
    EXPECT_EQ(Plane::xy, Plane(Coordinate(0, 0, 0), Vector(1, 0, 0), Vector(0, 1, 0)));
    EXPECT_EQ(0, Plane::xy.Distance(Coordinate::zero));
    EXPECT_DOUBLE_EQ(1, Plane::xy.Distance(Coordinate(0, 0, 1)));
    EXPECT_DOUBLE_EQ(0, Plane::xy.Distance(Coordinate(1, 1, 0)));
    EXPECT_3D_EQ(Coordinate::zero, Plane::xy.Project(Coordinate(0, 0, 1)));
    EXPECT_3D_EQ(Coordinate(1, 1, 0), Plane::xy.Project(Coordinate(1, 1, 1)));
    EXPECT_TRUE(Plane::xy.Above(Coordinate(0, 0, 1)));
    EXPECT_FALSE(Plane::xy.Above(Coordinate(0, 0, -1)));
}


TEST(Plane, Diagonal)
{
    Plane plane(Coordinate(1, 0, 0), Coordinate(0, 1, 0), Coordinate(0, 0, 1));
    EXPECT_TRUE(plane);
    // as long as it's clockwise the same, the plane is the same
    EXPECT_EQ(plane, Plane(Coordinate(0, 0, 1), Coordinate(1, 0, 0), Coordinate(0, 1, 0)));
    EXPECT_EQ(plane, Plane(Coordinate(0, 1, 0), Coordinate(0, 0, 1), Coordinate(1, 0, 0)));
    // flipped direction is flipped plane 
    EXPECT_EQ(-plane, Plane(Coordinate(1, 0, 0), Coordinate(0, 0, 1), Coordinate(0, 1, 0)));

    EXPECT_EQ(0, plane.Distance(Coordinate(1, 0, 0)));
    EXPECT_EQ(0, plane.Distance(Coordinate(0, 1, 0)));
    EXPECT_EQ(0, plane.Distance(Coordinate(0, 0, 1)));
    EXPECT_DOUBLE_EQ(-1.0 / sqrt(3), plane.Distance(Coordinate::zero));

    EXPECT_3D_EQ(Vector(1.0 / 3.0, 1.0 / 3.0, 1.0 / 3.0), plane.Project(Coordinate::zero));
}

TEST(Plane, DoubleXYAt1)
{
    Plane xy2(Coordinate(1,1,1), Vector(0, 0, 2));
    EXPECT_TRUE(xy2.Above(Coordinate(1, 1, 1.1)));
    EXPECT_FALSE(xy2.Above(Coordinate(0, 0, 0.9)));
    EXPECT_DOUBLE_EQ( 1, xy2.Distance(Coordinate(1,1,2)));
    EXPECT_DOUBLE_EQ(-1, xy2.Distance(Coordinate::zero));
    EXPECT_DOUBLE_EQ(-2, xy2.Distance(Coordinate(1,1,-1)));
    EXPECT_3D_EQ(Coordinate(0, 0, 1), xy2.Project(Coordinate::zero));
    EXPECT_3D_EQ(Coordinate(1, 1, 1), xy2.Project(Coordinate(1, 1, 1)));
    EXPECT_3D_EQ(Coordinate(-2,-3, 1), xy2.Project(Coordinate(-2, -3, -4)));
    // bounding box should be inf, inf, 1 ... inf
    EXPECT_TRUE(xy2.GetBoundingBox().x.infinite());
    EXPECT_TRUE(xy2.GetBoundingBox().y.infinite());
    EXPECT_FALSE(xy2.GetBoundingBox().Contains(Coordinate(1, 1, 1.1)));
    EXPECT_TRUE(xy2.GetBoundingBox().Contains(Coordinate(0, 0, 0.9)));
}


TEST(Plane, NotNormalDiagonal)
{
    Plane abnormal(Coordinate(1,0,0), Vector(-2, 2, 0), Vector(0, 2, -2));
    EXPECT_TRUE(abnormal);
    EXPECT_EQ(abnormal, abnormal.Normalized());
    EXPECT_EQ(0, abnormal.Distance(Coordinate(1, 0, 0)));
    EXPECT_EQ(0, abnormal.Distance(Coordinate(-1, 2, 0)));
    EXPECT_EQ(0, abnormal.Distance(Coordinate(1, 2, -2)));
    EXPECT_3D_EQ(Vector(1.0 / 3.0, 1.0 / 3.0, 1.0 / 3.0), abnormal.Project(Coordinate::zero));
    EXPECT_TRUE(abnormal.GetBoundingBox().x.infinite());
    EXPECT_TRUE(abnormal.GetBoundingBox().y.infinite());
    EXPECT_TRUE(abnormal.GetBoundingBox().z.infinite());

}

TEST(Triangle, Surface)
{
    Triangle point(Coordinate(1, 1, 1), Coordinate(1, 1, 1), Coordinate(1, 1, 1));
    EXPECT_EQ(0,point.Surface());

    Triangle splinter(Coordinate(1, 1, 1), Coordinate(2, 2, 2), Coordinate(3, 3, 3));
    EXPECT_EQ(0, splinter.Surface());

    Triangle xyTriangle(Coordinate(1, 0, 0), Coordinate(3, 0, 0), Coordinate(1, 4, 0));
    EXPECT_EQ(4, xyTriangle.Surface());

    Triangle yzTriangle(Coordinate(0, 0, 0), Coordinate(0, 1, 0), Coordinate(0, 0, 2));
    EXPECT_EQ(1, yzTriangle.Surface());

    Triangle xzTriangle(Coordinate(0, 0, 0), Coordinate(0, 0, 3), Coordinate(0, 2, 1));
    EXPECT_EQ(3, xzTriangle.Surface());
}

TEST(Triangle, Distance)
{
    Triangle point(Coordinate(1, 1, 1), Coordinate(1, 1, 1), Coordinate(1, 1, 1));
    EXPECT_EQ(1, point.Distance(Coordinate(0, 1, 1)));
    EXPECT_EQ(1, point.Distance(Coordinate(1, 1, 2)));

    Triangle splinter(Coordinate(1, 1, 1), Coordinate(2, 2, 2), Coordinate(3, 3, 3));
    EXPECT_EQ(1, splinter.Distance(Coordinate(0, 1, 1)));
    EXPECT_EQ(2, splinter.Distance(Coordinate(3, 5, 3)));

    // right hand ccw (like default opengl): triangle is facing -z axis
    Triangle xyTriangle(Coordinate(1, 4, 0), Coordinate(3, 0, 0), Coordinate(1, 0, 0));
    EXPECT_LT(xyTriangle.Normal().z, 0);
    EXPECT_EQ(1, xyTriangle.Distance(Coordinate(0, 0, 0))); // near first vertex
    EXPECT_EQ(3, xyTriangle.Distance(Coordinate(3, 0, -3))); // near second vertex, but in front
    EXPECT_EQ(2, xyTriangle.Distance(Coordinate(1, 6, 0))); // near third vertex
    EXPECT_EQ(0, xyTriangle.Distance(Coordinate(2, 0, 0))); // on edge
    EXPECT_EQ(1, xyTriangle.Distance(Coordinate(2, -1, 0))); // near edge
    EXPECT_EQ(0, xyTriangle.Distance(Coordinate(2, 2, 0))); // in triangle
    EXPECT_EQ(2, xyTriangle.Distance(Coordinate(2, 1, -2))); // in front of triangle
    EXPECT_EQ(-2, xyTriangle.Distance(Coordinate(2, 1, 2))); // behind triangle
    EXPECT_EQ(2, xyTriangle.Flipped().Distance(Coordinate(2, 1, 2))); // flipped triangle->flipped distance

    // triangle is facing +y axis
    Triangle xzTriangle(Coordinate(0, 0, 0), Coordinate(0, 0, 3), Coordinate(2, 0, 1));
    EXPECT_GT(xzTriangle.Normal().y, 0);
    EXPECT_EQ(-1, xzTriangle.Distance(Coordinate(0, -1, 0))); // behind first vertex
    EXPECT_EQ(1, xzTriangle.Distance(Coordinate(3, 0, 1))); // in front of third vertex
    EXPECT_EQ(0, xzTriangle.Distance(Coordinate(0, 0, 2.5))); // on first edge
    EXPECT_EQ(-2, xzTriangle.Distance(Coordinate(0, -2, 2))); // near first edege
}

}
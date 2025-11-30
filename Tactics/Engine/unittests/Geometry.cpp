#include <gtest/gtest.h>
#include "Geometry/Angles.h"
#include "Geometry/Coordinate.h"
#include "Geometry/Vector.h"
#include "Geometry/Quaternion.h"
#include "Geometry/Line.h"
#include "Geometry/Plane.h"
#include "Geometry/Triangle.h"
#include "Geometry/AxisAlignedBoundingBox.h"
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

const double maxError = 0.001;

TEST(Angles, Degrees)
{
    EXPECT_EQ(Engine::Rad2Deg(0), 0);
    EXPECT_NEAR(Engine::Rad2Deg(Engine::PI), 180, maxError);
    EXPECT_NEAR(Engine::Rad2Deg(-Engine::PI), -180, maxError);
    EXPECT_NEAR(Engine::Rad2Deg(2 * Engine::PI), 360, maxError);
}


TEST(Angles, Turn)
{
    double halfpi = Engine::PI * 0.5;
    EXPECT_NEAR(Engine::ShortestTurn(0, halfpi), halfpi, maxError);
    EXPECT_NEAR(Engine::ShortestTurn(0, halfpi), halfpi, maxError);
    EXPECT_NEAR(Engine::ShortestTurn(halfpi, 0), -halfpi, maxError);
    EXPECT_NEAR(Engine::ShortestTurn(halfpi, Engine::PI), halfpi, maxError);
    EXPECT_NEAR(Engine::ShortestTurn(0.75*Engine::PI, -0.75*Engine::PI), halfpi, maxError);
    EXPECT_NEAR(Engine::ShortestTurn(-0.75*Engine::PI, 0.75*Engine::PI), -halfpi, maxError);
}

TEST(Angles, FaceYawPitch)
{
    Vector forward{0,1,0};
    EXPECT_EQ(FaceYawPitch(forward), std::make_pair(0.0, 0.0));
    EXPECT_EQ(FaceYawPitch(Vector( 1, 0, 0)), std::make_pair(PI/2, 0.0));
    EXPECT_EQ(FaceYawPitch(Vector(-1, 0, 0)), std::make_pair(-PI/2, 0.0));
    EXPECT_EQ(FaceYawPitch(Vector( 0, 0, 1)), std::make_pair(0.0, PI/2));

    EXPECT_EQ(FaceYawPitch(Quaternion(Vector(0,0,1), 1.0) * forward), std::make_pair(-1.0, 0.0));
    EXPECT_EQ(FaceYawPitch(Quaternion(Vector(1,0,0), -0.5) * forward), std::make_pair(0.0, -0.5));
    auto yawPitchZAxis = FaceYawPitch(Quaternion(Vector(0,0,1), -0.5) * Quaternion(Vector(1,0,0), 1.0) * forward);
    EXPECT_DOUBLE_EQ(yawPitchZAxis.first, 0.5);
    EXPECT_DOUBLE_EQ(yawPitchZAxis.second, 1.0);
/*  // TODO no idea why the sign of these angles doesn't match or why yaw is inverted but pitch is not in the tests above
    auto yawPitchYAxis = FaceYawPitch(Quaternion(Vector(0,1,0), -0.5) * Quaternion(Vector(1,0,0), 1.0) * Vector(0, 0, 1), false);
    EXPECT_DOUBLE_EQ(yawPitchYAxis.first, -0.5);
    EXPECT_DOUBLE_EQ(yawPitchYAxis.second, -1.0);
*/
}

TEST(Line, Length)
{
    Coordinate a(1, 2, 3);
    Coordinate b(3, 0, 4);
    EXPECT_EQ(3.0, Line( a, b ).Length());
}

TEST(Line, Distance)
{
    Line l0(Coordinate(1, 0, 0), Coordinate(1, 0, 0));
    EXPECT_EQ(1.0, l0.Distance(Coordinate(1, -1, 0)));

    Line l1(Coordinate(1, 0, 0), Coordinate(3, 2, 0));
    EXPECT_EQ(l1.Distance(Coordinate(1, -1, 0)), 1.0);
    EXPECT_EQ(l1.Distance(Coordinate(5, 2, 0)), 2.0);
    EXPECT_EQ(l1.Distance(Coordinate(0, 3, 1)), 3.0);

    Line l2(Coordinate(1, 1, 0), Coordinate(1, 1, 0));
    EXPECT_EQ(1.0, l2.Distance(Coordinate(1, 0, 0)));
}



TEST(Line, Section)
{
    Coordinate a(1, 2, 3);
    Coordinate b(3, 0, 4);
    Line line(a, b);
    auto firstSection = line.Section(Range(0.0, line.Length() / 2.0));
    EXPECT_EQ(line.a, firstSection.a);
    EXPECT_EQ(firstSection.b, Coordinate(2, 1, 3.5));
    auto secondSection = line.Section(Range(line.Length() / 2.0, line.Length()));
    EXPECT_EQ(line.b, secondSection.b);
    EXPECT_EQ(secondSection.a, firstSection.b);
    auto clippedSection = line.Section(Range(0.0, std::numeric_limits<double>::infinity()));
    EXPECT_EQ(clippedSection, line);

}

TEST(Line, Vozelize)
{
    Coordinate a(0.5, 2.2, 3.6);
    Coordinate b(3.1, -0.5, -2.0);

    //     Y                 Za
    //     3                 3*
    //     2a*               2 *
    //     1  **             1  *
    //X -1 0 1 2*3        X 0 1 2*3
    //    -1     *b         -1    *
    //                      -2     b

    Line line(a,b);
    auto voxels = line.Voxelize();
    EXPECT_EQ(voxels[0].first, Position(0, 2, 3));
    EXPECT_EQ(voxels[1].first, Position(1, 2, 3));
    auto total_fragments = std::accumulate(voxels.begin(), voxels.end(), 0.0,
        [](double sum, std::pair<Position, double> voxel)
    {
        return sum + voxel.second;
    });
    EXPECT_DOUBLE_EQ(total_fragments, line.Length());

}


TEST(Plane, Null)
{
    EXPECT_FALSE(Plane(Vector::zero, 1));
    EXPECT_FALSE(Plane(Vector::zero, 1));
    EXPECT_FALSE(Plane(Coordinate::origin, Coordinate::origin, Coordinate::origin));
    EXPECT_FALSE(Plane(Coordinate(1, 0, 0), Coordinate(1, 0, 0), Coordinate(1, 0, 0)));
    EXPECT_EQ(Plane(Vector::zero,0) , Plane(Coordinate(1, 0, 0), Coordinate(1, 0, 0), Coordinate(1, 0, 0)));
    EXPECT_FALSE(Plane::null);
    EXPECT_FALSE(Plane::null.Normalized());
}

TEST(Plane, XY)
{
    EXPECT_TRUE(Plane::xy);
    EXPECT_EQ(Plane::xy, Plane(Coordinate::origin, Vector::X, Vector::Y));
    EXPECT_EQ(0, Plane::xy.Distance(Coordinate::origin));
    EXPECT_DOUBLE_EQ(1, Plane::xy.Distance(Coordinate(0, 0, 1)));
    EXPECT_DOUBLE_EQ(0, Plane::xy.Distance(Coordinate(1, 1, 0)));
    EXPECT_EQ(Coordinate::origin, Plane::xy.Project(Coordinate(0, 0, 1)));
    EXPECT_EQ(Coordinate(1, 1, 0), Plane::xy.Project(Coordinate(1, 1, 1)));
    EXPECT_TRUE(Plane::xy.Above(Coordinate(0, 0, 1)));
    EXPECT_FALSE(Plane::xy.Above(Coordinate(0, 0, -1)));
}

TEST(Plane, XYIntersection)
{
    EXPECT_EQ(Plane::xy.Intersection(Line(Coordinate(0,0,2), Coordinate(0, 0, -6))).begin, 2);
    EXPECT_EQ(Plane::xy.Intersection(Line(Coordinate(0,0,-6), Coordinate(0, 0, 2))).begin, -6);
    EXPECT_FALSE(Plane::xy.Intersection(Line(Coordinate(0,1,2), Coordinate(1, 0, 2))));
    EXPECT_FALSE(Plane::xy.Intersection(Line(Coordinate(0,0,2), Coordinate(0, 0, 1))));
    EXPECT_FALSE(Plane::xy.Intersection(Line(Coordinate(0,0,-2), Coordinate(0, 0, -4))));
}


TEST(Plane, Diagonal)
{
    Plane plane(Coordinate(1, 0, 0), Coordinate(0, 1, 0), Coordinate(0, 0, 1));
    EXPECT_TRUE(plane);
    // as long as it's clockwise the same, the plane is the same
    EXPECT_EQ(Plane(Coordinate(0, 0, 1), Coordinate(1, 0, 0), Coordinate(0, 1, 0)), plane);
    EXPECT_EQ(Plane(Coordinate(0, 1, 0), Coordinate(0, 0, 1), Coordinate(1, 0, 0)), plane);
    // flipped direction is flipped plane
    EXPECT_EQ(Plane(Coordinate(1, 0, 0), Coordinate(0, 0, 1), Coordinate(0, 1, 0)), -plane);

    EXPECT_EQ(plane.Distance(Coordinate(1, 0, 0)), 0);
    EXPECT_EQ(plane.Distance(Coordinate(0, 1, 0)), 0);
    EXPECT_EQ(plane.Distance(Coordinate(0, 0, 1)), 0);
    EXPECT_DOUBLE_EQ(-1.0 / sqrt(3), plane.Distance(Coordinate::origin));
    EXPECT_EQ(plane.Project(Coordinate::origin), Coordinate(1.0 / 3.0, 1.0 / 3.0, 1.0 / 3.0));
}

TEST(Plane, NotNormalXYAt1)
{
    Plane xy2(Coordinate(1,1,1), Vector(0, 0, 2));
    EXPECT_TRUE(xy2.Above(Coordinate(1, 1, 1.1)));
    EXPECT_FALSE(xy2.Above(Coordinate(0, 0, 0.9)));
    EXPECT_DOUBLE_EQ(xy2.Distance(Coordinate(1,1,2)), 1);
    EXPECT_DOUBLE_EQ(xy2.Distance(Coordinate::origin), -1);
    EXPECT_DOUBLE_EQ(xy2.Distance(Coordinate(1,1,-1)), -2);
    EXPECT_EQ(Coordinate(0, 0, 1), xy2.Project(Coordinate::origin));
    EXPECT_EQ(Coordinate(1, 1, 1), xy2.Project(Coordinate(1, 1, 1)));
    EXPECT_EQ(Coordinate(-2,-3, 1), xy2.Project(Coordinate(-2, -3, -4)));
    // bounding box should be inf, inf, 1 ... inf
    EXPECT_TRUE(xy2.GetBoundingBox().x.infinite());
    EXPECT_TRUE(xy2.GetBoundingBox().y.infinite());
    EXPECT_FALSE(xy2.GetBoundingBox().Contains(Coordinate(1, 1, 1.1)));
    EXPECT_TRUE(xy2.GetBoundingBox().Contains(Coordinate(0, 0, 0.9)));

    EXPECT_EQ(xy2.Intersection(Line(Coordinate(0,0,2), Coordinate(0, 0, 0))).begin, 1);

}


TEST(Plane, NotNormalDiagonal)
{
    Plane abnormal(Coordinate(1,0,0), Vector(-2, 2, 0), Vector(0, 2, -2));
    EXPECT_TRUE(abnormal);
    EXPECT_EQ(abnormal.Normalized(), abnormal);
    EXPECT_EQ(abnormal.Distance(Coordinate(1, 0, 0)), 0);
    EXPECT_EQ(abnormal.Distance(Coordinate(-1, 2, 0)), 0);
    EXPECT_EQ(abnormal.Distance(Coordinate(1, 2, -2)), 0);
    EXPECT_EQ(abnormal.Project(Coordinate::origin), Coordinate(1.0/3.0, 1.0/3.0, 1.0/3.0));
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

    Triangle yzTriangle(Coordinate::origin, Coordinate(0, 1, 0), Coordinate(0, 0, 2));
    EXPECT_EQ(1, yzTriangle.Surface());

    Triangle xzTriangle(Coordinate::origin, Coordinate(0, 0, 3), Coordinate(0, 2, 1));
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
    EXPECT_EQ(1, xyTriangle.Distance(Coordinate::origin)); // near first vertex
    EXPECT_EQ(3, xyTriangle.Distance(Coordinate(3, 0, -3))); // near second vertex, but in front
    EXPECT_EQ(2, xyTriangle.Distance(Coordinate(1, 6, 0))); // near third vertex
    EXPECT_EQ(0, xyTriangle.Distance(Coordinate(2, 0, 0))); // on edge
    EXPECT_EQ(1, xyTriangle.Distance(Coordinate(2, -1, 0))); // near edge
    EXPECT_EQ(0, xyTriangle.Distance(Coordinate(2, 2, 0))); // in triangle
    EXPECT_EQ(2, xyTriangle.Distance(Coordinate(2, 1, -2))); // in front of triangle
    EXPECT_EQ(-2, xyTriangle.Distance(Coordinate(2, 1, 2))); // behind triangle
    EXPECT_EQ(2, xyTriangle.Flipped().Distance(Coordinate(2, 1, 2))); // flipped triangle->flipped distance

    // triangle is facing +y axis
    Triangle xzTriangle(Coordinate::origin, Coordinate(0, 0, 3), Coordinate(2, 0, 1));
    EXPECT_GT(xzTriangle.Normal().y, 0);
    EXPECT_EQ(-1, xzTriangle.Distance(Coordinate(0, -1, 0))); // behind first vertex
    EXPECT_EQ(1, xzTriangle.Distance(Coordinate(3, 0, 1))); // in front of third vertex
    EXPECT_EQ(0, xzTriangle.Distance(Coordinate(0, 0, 2.5))); // on first edge
    EXPECT_EQ(-2, xzTriangle.Distance(Coordinate(0, -2, 2))); // near first edege
}

TEST(Triangle, Intersection)
{
    Triangle xyTriangle(Coordinate(0, 2, 0), Coordinate(3, 0, 0), Coordinate(1, 4, 0));
    EXPECT_NEAR(xyTriangle.Intersection(Line(Coordinate(2,1,2), Coordinate(2,1,-1))), 2.0, 0.01);
    EXPECT_NEAR(xyTriangle.Intersection(Line(Coordinate(2,1,-1), Coordinate(2,1,2))), -1, 0.01);
    EXPECT_EQ(xyTriangle.Intersection(Line(Coordinate(1,2,1.5), Coordinate(1,2,0))), 1.5);
    EXPECT_EQ(xyTriangle.Intersection(Line(Coordinate(1,2,0), Coordinate(1,2,-2))), 0.0);
    EXPECT_NEAR(xyTriangle.Intersection(Line(Coordinate(1,2,1), Coordinate(0,2,0))), sqrt(2), 0.00001);
    EXPECT_TRUE(std::isnan(xyTriangle.Intersection(Line(Coordinate(1,2,1), Coordinate(0,2,1))))) << "parallel line intersected the triangle";
    EXPECT_TRUE(std::isnan(xyTriangle.Intersection(Line(Coordinate(1,1,1), Coordinate(1,1,-1))))) << "line didn't miss the triangle";
}

}

#include "pch.h"
#include "Volume.h"
#include "Mesh.h"
#include "Matrix.h"
#include "AxisAlignedBoundingBox.h"
#include "GTestGeometry.h"

namespace Engine::Test
{

TEST(Volume, Point)
{
    Point p(Coordinate(1, 1, 1));
    EXPECT_EQ(0, p.GetBoundingBox().Volume());
    EXPECT_EQ(1, p.Distance(Coordinate(0, 1, 1)));
}

TEST(Volume , Sphere)
{
    Sphere s(Coordinate(1, 1, 1), 1);
    EXPECT_3D_EQ(Vector(2,2,2), s.GetBoundingBox().Extent());
    EXPECT_TRUE(s.GetBoundingBox().Contains(Coordinate(0, 1, 1)));
    EXPECT_EQ(0, s.Distance(Coordinate(0, 1, 1)));
    EXPECT_EQ(-1, s.Distance(Coordinate(1, 1, 1)));
    EXPECT_EQ(2, s.Distance(Coordinate(1, 1, 4)));
}


TEST(Volume, AABox)
{
    AABB b(Coordinate(-1.0, 0.0, -1.0), Coordinate(2, 1, 0));
    EXPECT_3D_EQ(Vector(3,1,1), b.GetBoundingBox().Extent());
    EXPECT_TRUE(b.GetBoundingBox().Contains(Coordinate(0, 0.5, -0.5)));
    EXPECT_EQ(0, b.Distance(Coordinate(0, 1, -1)));
    EXPECT_EQ(-0.5, b.Distance(Coordinate(0, 0.5, -0.5)));
    EXPECT_EQ(1, b.Distance(Coordinate(1, 1, 1)));
}

TEST(Volume, InfiniteAABox)
{
    AABB b(Range<double>::infinity(), Range<double>::infinity(), Range<double>(-2,0));
    EXPECT_TRUE(std::isinf(b.GetBoundingBox().Volume()));
    EXPECT_EQ(2, b.Distance(Coordinate(0, 0, 2)));
    EXPECT_EQ(0, b.Distance(Coordinate::origin));
    EXPECT_EQ(-1, b.Distance(Coordinate(0, 0, -1)));
}


TEST(Volume, Box)
{
    Box b(Vector(2, 3, 4));
    b *= Matrix::Translation(Vector(1, 1.5, 2)); // Box is centered around 0,0,0
    b *= Matrix::Translation(Vector(1, -1, 0 )); // [1,3],[-1,2],[0,4]
    EXPECT_3D_EQ(Vector(2,3,4), b.GetBoundingBox().Extent());
    // On corners
    EXPECT_DOUBLE_EQ(0, b.Distance(Coordinate(1, -1, 4)));
    EXPECT_DOUBLE_EQ(0, b.Distance(Coordinate(3, 2, 0)));
    // On faces
    EXPECT_DOUBLE_EQ(0, b.Distance(Coordinate(2, -1, 0)));
    EXPECT_DOUBLE_EQ(0, b.Distance(Coordinate(3, 0, 4)));
    // Outside
    EXPECT_DOUBLE_EQ(1, b.Distance(Coordinate(0, 0, 2)));
    EXPECT_DOUBLE_EQ(2, b.Distance(Coordinate(2, 4, 3)));
    // Inside
    EXPECT_DOUBLE_EQ(-1, b.Distance(Coordinate(2, 0, 2)));
}

TEST(Cylinder, Nul)
{
    Cylinder c(Line(Coordinate::origin, Coordinate::origin), 0, 0);
    EXPECT_FALSE(c.Volume());
    EXPECT_EQ(0, c.Distance(Coordinate::origin));
    EXPECT_EQ(1.0, c.Distance(Coordinate(1, 0, 0)));

    EXPECT_FALSE(c.Slice(Range(0.0, 1.0)).Volume());

    EXPECT_3D_EQ(c.Axis().a, Coordinate::origin);
    EXPECT_3D_EQ(c.Axis().b, Coordinate::origin);
}

TEST(Cylinder, AxisAligned)
{
    Coordinate b(1, 0, 0);
    Cylinder c(Line(Coordinate::origin, b), 1, 1);
    EXPECT_DOUBLE_EQ(1.0*2.0*2.0, c.GetBoundingBox().Volume()); 
    EXPECT_EQ(0, c.Distance(Coordinate::origin));
    EXPECT_EQ(0.0, c.Distance(b));
    EXPECT_EQ(0.0, c.Distance(Coordinate(0, 1, 0)));
    EXPECT_EQ(0.0, c.Distance(Coordinate(0, 0, 1)));
    EXPECT_EQ(1, c.Distance(Coordinate(-1, 0, 0)));
    EXPECT_EQ(1, c.Distance(Coordinate( 0, -2, 0)));
    EXPECT_EQ(1, c.Distance(Coordinate(0, 0, 2)));
    EXPECT_DOUBLE_EQ(sqrt(2) - 1.0, c.Distance(Coordinate(0.5, 1, 1)));
    EXPECT_DOUBLE_EQ(sqrt(2) - 1.0, c.Distance(Coordinate(0, 1, 1)));
    EXPECT_DOUBLE_EQ(sqrt(2) - 1.0, c.Distance(Coordinate(1, 1, 1)));

    EXPECT_3D_EQ(c.Axis().a, Coordinate::origin);
    EXPECT_3D_NEAR(c.Axis().b, b, 1e-20);
}

TEST(Cylinder, Turned)
{
    Line axis(Coordinate::origin, Coordinate(2, 0, 0));
    Cylinder c(axis, 1, 1);
    EXPECT_3D_EQ(Vector(2,2,2), c.GetBoundingBox().Extent()); 

    EXPECT_3D_EQ(c.Axis().a, axis.a);
    EXPECT_3D_NEAR(c.Axis().b, axis.b, 1e-12);

}


TEST(Cylinder, Titlted)
{
    // cylinder still unit length
    Line axis(Coordinate::origin, Coordinate(0.5*sqrt(2), 0, 0.5*sqrt(2)));
    Cylinder c(axis, 0.5, 0.5);
    EXPECT_3D_EQ(Vector(1*sqrt(2), 1, 1*sqrt(2)),c.GetBoundingBox().Extent());
    EXPECT_LT(c.Distance(Coordinate(0.7, 0, 0.7)), 0.0);
    EXPECT_GT(c.Distance(Coordinate(1.0, 0, 0.0)), 0.0);

    EXPECT_3D_EQ(c.Axis().a, axis.a);
    EXPECT_3D_NEAR(c.Axis().b, axis.b, 1e-12);

}

TEST(Cylinder, Diagonal)
{
    Line axis(Coordinate::origin, Coordinate(2, 2, 0));
    Cylinder c(axis, 1, 1);
    EXPECT_3D_EQ(Vector(2+sqrt(2),2+sqrt(2), 2), c.GetBoundingBox().Extent());

    EXPECT_3D_EQ(c.Axis().a, axis.a);
    EXPECT_3D_NEAR(c.Axis().b, axis.b, 1e-12);
}

TEST(Cylinder, Ellipse)
{
    Cylinder flat(Line(Coordinate::origin, Coordinate(2, 0, 0)), 1, 0.0);
    EXPECT_DOUBLE_EQ(0.0, flat.GetBoundingBox().Volume()); 
    EXPECT_DOUBLE_EQ(0.0, flat.Distance(Coordinate(1.0, 0.0, 0.0)));
    EXPECT_DOUBLE_EQ(1.0, flat.Distance(Coordinate(1.0, 0.0, 1.0)));
    EXPECT_DOUBLE_EQ(0.0, flat.Distance(Coordinate(1.0, 1.0, 0.0)));

    Cylinder skinny(Line(Coordinate::origin, Coordinate(2, 0, 0)), 0.5, 1.0);
    EXPECT_3D_EQ(Vector(2,1,2), skinny.GetBoundingBox().Extent());

    Cylinder skinnz(Line(Coordinate::origin, Coordinate(2, 0, 0)), 1, 0.5);
    EXPECT_3D_EQ(Vector(2,2,1), skinnz.GetBoundingBox().Extent());
}

TEST(Intersection, Boxes)
{
    AABB a(Coordinate(0.0, 0.0, 0.0), Coordinate(2, 3, 4));
    AABB b(Coordinate(-3, -2, -1), Coordinate(1, 1, 2));
    Intersection i({ a, b });
    EXPECT_3D_EQ(Vector(1,1,2), i.GetBoundingBox().Extent());
    EXPECT_DOUBLE_EQ(-0.5, i.Distance(Coordinate(0.5, 0.5, 0.5)));
    EXPECT_DOUBLE_EQ( 0.0, i.Distance(Coordinate(1.0, 1.0, 0.5)));
    EXPECT_DOUBLE_EQ(-0.5, i.Distance(Coordinate(0.5, 0.5, 0.5)));
    EXPECT_DOUBLE_EQ( 1.0, i.Distance(Coordinate(-1.0, 0.5, 0.5)));
}

}

#include <gtest/gtest.h>
#include "Geometry/Mesh.h"
#include "Geometry/AxisAlignedBoundingBox.h"
#include "Rendering/OffscreenSurface.h"
#include "Rendering/Color.h"
#include "Geometry/Matrix.h"
#include "GTestGeometry.h"

namespace Engine::Test
{

struct Mesh : public ::testing::Test
{
    Mesh() = default;
    OffscreenSurface surface;
    const double precision = 1e-12;
};

TEST_F(Mesh, BoundingBox)
{
    Box cube(1.0);
    EXPECT_EQ(Vector(1.0, 1.0, 1.0), cube.GetBoundingBox().Extent());
    EXPECT_TRUE(cube.GetBoundingBox().Contains(Engine::Coordinate::origin));
}

TEST_F(Mesh, BoxVolume)
{   // Test IVolume interface with bounding box
    Box cube(2.0);
    EXPECT_NEAR(8.0, cube.GetBoundingBox().Volume(), precision);
    // and exact distance to surface
    Coordinate center(1, 1, 1);
    EXPECT_EQ(-1.0, cube.Distance(center));
    EXPECT_EQ(-0.5, cube.Distance(center + Vector(0.5, 0, 0)));
    EXPECT_EQ(-0.25, cube.Distance(center + Vector(0, 0.75, -0.75)));

    std::vector<Vector> dirs = { Vector(1,0,0),Vector(0,1,0), Vector(0,0,1), Vector(-1,0,0), Vector(0,-1,0), Vector(0,0,-1) };
    for (auto dir : dirs)
    {
        EXPECT_DOUBLE_EQ(0, cube.Distance(center+dir)) << dir;
        EXPECT_DOUBLE_EQ(1, cube.Distance(center + dir * 2.0)) << dir;
    }
}

TEST_F(Mesh, Render)
{
    Box cube(1.0);
    cube.Render();
    cube.Invalidate();
    cube.Render();
}

TEST_F(Mesh, Scale)
{
    Box cube(1.0);
    cube *= Matrix::Scale(Engine::Vector(2, 2, 2));
    EXPECT_EQ(Vector(2,2,2), cube.GetBoundingBox().Extent());
}

TEST_F(Mesh, Translate)
{
    Box cube(1.0);
    cube *= Matrix::Translation(Engine::Vector(1, 1, 1));
    EXPECT_FALSE(cube.GetBoundingBox().Contains(Engine::Coordinate::origin));
}

TEST_F(Mesh, Color)
{
    Box cube;
    cube.SetColor(RGBA::red);
    EXPECT_TRUE(all_of(cube.vertices.begin(), cube.vertices.end(), [](const Engine::Mesh::Vertex& v)
    {
        return v.color == RGBA::red;
    }));
}

TEST_F(Mesh, ColorNamed)
{
    Engine::Mesh mesh;
    Box cube;
    cube.SetColor(RGBA::white);
    cube.SetName(1);
    mesh += cube;
    Box otherCube(2.0);
    otherCube.SetColor(RGBA::red);
    otherCube.SetName(2);
    mesh += otherCube;

    mesh.SetColor(2, RGBA::green);
    ASSERT_FALSE(all_of(mesh.vertices.begin(), mesh.vertices.end(), [](const Engine::Mesh::Vertex& v)
    {
        return v.color == RGBA::green;
    }));

    mesh.SetColor(RGBA::green);
    EXPECT_TRUE(all_of(mesh.vertices.begin(), mesh.vertices.end(), [](const Engine::Mesh::Vertex& v)
    {
        return v.color == RGBA::green;
    }));
}

}

#include "pch.h"
#include "Mesh.h"
#include "AxisAlignedBoundingBox.h"
#include "OffscreenSurface.h"
#include "Matrix.h"
#include <gl/glew.h>

namespace Engine::Test
{

struct TestMesh : public ::testing::Test
{
    TestMesh() = default;
    OffscreenSurface surface;
};

TEST_F(TestMesh, BoundingBox)
{
    Box cube(1.0);
    EXPECT_NEAR(1.0, cube.GetBoundingBox().Volume(), 1e-6);
    EXPECT_TRUE(cube.GetBoundingBox().Contains(Engine::Coordinate(0, 0, 0)));
}

TEST_F(TestMesh, Render)
{
    Box cube(1.0);
    cube.Render();
    cube.Invalidate();
    cube.Render();
}

TEST_F(TestMesh, Scale)
{
    Box cube(1.0);
    cube *= Matrix::Scale(Engine::Vector(2, 2, 2));
    EXPECT_NEAR(8.0, cube.GetBoundingBox().Volume(), 1e-6);
}

TEST_F(TestMesh, Translate)
{
    Box cube(1.0);
    cube *= Matrix::Translation(Engine::Vector(1, 1, 1));
    EXPECT_FALSE(cube.GetBoundingBox().Contains(Engine::Coordinate(0, 0, 0)));
}

}

#include "pch.h"
#include "Mesh.h"
#include "AxisAlignedBoundingBox.h"
#include "OffscreenSurface.h"
#include <gl/glew.h>

namespace Engine::Test
{

struct TestMesh : public ::testing::Test
{
    TestMesh()
    {
        
    }
    OffscreenSurface surface;
};

TEST_F(TestMesh, BoundingBox)
{
    Box cube(1.0);
    EXPECT_NEAR(1.0, cube.GetBoundingBox().Volume(), 1e-6);
}

}

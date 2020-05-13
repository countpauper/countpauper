#include "pch.h"
#include "Mesh.h"
#include "AxisAlignedBoundingBox.h"

namespace Engine::Test
{

TEST(Mesh, BoundingBox)
{
    Box cube(1.0);
    EXPECT_NEAR(1.0, cube.GetBoundingBox().Volume(), 1e-6);
}

}

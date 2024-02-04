#include <gtest/gtest.h>
#include "Engine/Effect.h"
#include "GTestGeometry.h"
#include <array>
#include <numeric>
#include <iostream>
namespace Engine::Test
{
  
    TEST(Effect, Cloud)
    {
        ParticleEffect<Cloud> cloud(1);
        cloud.Tick(0);
        EXPECT_EQ(cloud.Count(), 1);
        EXPECT_LE(cloud.Bounds().Extent().z, 2);
        cloud.Tick(0.5);
        EXPECT_GT(cloud.Bounds().z.Size(),0);
        cloud.Tick(0.6);
        cloud.Tick(0);
        EXPECT_EQ(cloud.Count(), 1);
        EXPECT_NO_THROW(cloud.Render());
        EXPECT_GT(cloud.Bounds().x.Size(), 0);
        EXPECT_GT(cloud.Bounds().y.Size(), 0);
    }

}

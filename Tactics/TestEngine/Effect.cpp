#include "pch.h"
#include <array>
#include <numeric>
#include <iostream>
#include "Effect.h"
#include "GTestGeometry.h"

namespace Engine::Test
{
  
    TEST(Effect, Cloud)
    {
        ParticleEffect<Cloud> cloud(1);
        cloud.Tick(0);
        EXPECT_EQ(1, cloud.Count());
        EXPECT_EQ(0, cloud.Bounds().Extent().z);
        cloud.Tick(0.5);
        EXPECT_GT(cloud.Bounds().z.Size(),0);
        cloud.Tick(0.6);
        cloud.Tick(0);
        EXPECT_EQ(1, cloud.Count());
        EXPECT_NO_THROW(cloud.Render());
        EXPECT_GT(cloud.Bounds().x.Size(), 0);
        EXPECT_GT(cloud.Bounds().y.Size(), 0);
    }



}

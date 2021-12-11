#include "pch.h"
#include <array>
#include <numeric>
#include <iostream>
#include "Effect.h"
#include "GTestGeometry.h"

namespace Engine::Test
{
  
    TEST(Effect, Steam)
    {
        ParticleEffect<Steam> steam;
        steam.Tick(0);
        EXPECT_EQ(1, steam.Count());
        EXPECT_3D_EQ(Vector(0,0,0), steam.Bounds().Extent());
        steam.Tick(0.5);
        EXPECT_GT(steam.Bounds().z.Size(),0);
        steam.Tick(0.6);
        steam.Tick(0);
        EXPECT_EQ(2, steam.Count());
        EXPECT_NO_THROW(steam.Render());
        EXPECT_GT(steam.Bounds().x.Size(), 0);
        EXPECT_GT(steam.Bounds().y.Size(), 0);
    }



}

#include "pch.h"
#include "Physics/PackedVoxel.h"

namespace Physics::Test
{

TEST(Packed, Temperature)
{
    PackedVoxel v;
    EXPECT_EQ(v.Temperature(), 0);
    for (double t = 0; t < 5000; t+=9.87654321)
    {
        v.SetTemperature(t);
        double accuracy = std::max(0.5, t*0.01);
        EXPECT_NEAR(t, v.Temperature(), accuracy);
        double reproduce = v.Temperature();
        v.SetTemperature(reproduce);
        EXPECT_EQ(v.Temperature(), reproduce);
    }
}

TEST(Packed, TemperatureAliasing)
{
    PackedVoxel v;
    EXPECT_EQ(v.Temperature(), 0);
    double totalError = 0;
    for (double t = 0; t < 350; t += 0.01)
    {
        v.SetTemperature(t);
        totalError += t - v.Temperature();
    }
    EXPECT_LE(totalError, 1);
}


}

#include "pch.h"
#include "Physics/ElementalVoxel.h"

namespace Physics::Test
{

    TEST(Elemental, TemperatureAccuracy)
    {
        ElementalVoxel v;
        EXPECT_EQ(v.Temperature(), 0);
        for (double t = 0; t <= 2550; t += 10.0)
        {
            v.SetTemperature(t);
            double accuracy = std::max(10.0, t / 100);
            EXPECT_NEAR(t, v.Temperature(), accuracy) << "at " << t << "K";
            double reproduce = v.Temperature();
            v.SetTemperature(reproduce);
            EXPECT_EQ(v.Temperature(), reproduce) << " at " << t << "K";
        }
    }


    TEST(Elemental, Air)
    {
        ElementalVoxel v(Material::air, 300);
        EXPECT_EQ(v.GetMaterial(), &Material::air);
        EXPECT_TRUE(v.IsGas());
    }

    TEST(Elemental, Stone)
    {
        ElementalVoxel v(Material::stone, 300);
        EXPECT_EQ(v.GetMaterial(), &Material::stone);
        EXPECT_TRUE(v.IsSolid());
    }

    TEST(Elemental, Water)
    {
        ElementalVoxel v(Material::water, 300);
        EXPECT_EQ(v.GetMaterial(), &Material::water);
        EXPECT_TRUE(v.IsFluid());
    }

    TEST(Elemental, Soil)
    {
        ElementalVoxel v(Material::soil, 300);
        EXPECT_EQ(v.GetMaterial(), &Material::soil);
        EXPECT_TRUE(v.IsSolid());
    }


}

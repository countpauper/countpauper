#include "pch.h"
#include "Physics/DiscreteGrid.h"
#include "Engine/Volume.h"

namespace Physics
{
namespace Test
{
TEST(DiscreteGrid, Space)
{
    DiscreteGrid nullEnv(Engine::Vector(0, 0, 0));
    EXPECT_EQ(nullptr, nullEnv.GetMaterial(Engine::Coordinate(0, 0, 0)));
    EXPECT_DOUBLE_EQ(0, nullEnv.Measure(&Material::vacuum));

    DiscreteGrid oneEnv(Engine::Vector(1, 1, 1));
    EXPECT_EQ(&Material::vacuum, oneEnv.GetMaterial(Engine::Coordinate(0, 0, 0)));
    EXPECT_DOUBLE_EQ(1.0, oneEnv.Measure(&Material::vacuum));
    EXPECT_EQ(nullptr, oneEnv.GetMaterial(Engine::Coordinate(-1, 0, 0)));
    EXPECT_EQ(nullptr, oneEnv.GetMaterial(Engine::Coordinate(0, 0, 2)));

    DiscreteGrid splitEnv(Engine::Vector(2, 2, 2));
    EXPECT_EQ(nullptr, splitEnv.GetMaterial(Engine::Coordinate(-1, 0, 0)));
    EXPECT_EQ(nullptr, splitEnv.GetMaterial(Engine::Coordinate(0, 3, 0)));
}

TEST(DiscreteGrid, Fill)
{
    DiscreteGrid nullEnv(Engine::Vector(0, 0, 0));
    nullEnv.Fill(Engine::Sphere(Engine::Coordinate(0, 0, 0), 1), fillAll, Physics::Material::air, 0);

    EXPECT_EQ(nullptr, nullEnv.GetMaterial(Engine::Coordinate(0, 0, 0)));

    DiscreteGrid oneEnv(Engine::Vector(1, 1, 1));
    oneEnv.Fill(Engine::Sphere(Engine::Coordinate(0, 0, 0), 1), fillAll, Material::water, 273);
    EXPECT_EQ(&Material::water, oneEnv.GetMaterial(Engine::Coordinate(0, 0, 0)));
    EXPECT_NEAR(273.0, oneEnv.Temperature(Engine::AABox(Engine::Coordinate(0, 0, 0), Engine::Coordinate(1, 1, 1))), 1);
    EXPECT_DOUBLE_EQ(1.0, oneEnv.Measure(&Material::water));
    EXPECT_DOUBLE_EQ(0.0, oneEnv.Measure(&Material::vacuum));

    DiscreteGrid quadEnv(Engine::Vector(2, 2, 2));
    EXPECT_DOUBLE_EQ(2 * 2 * 2, quadEnv.Measure(&Material::vacuum));
    quadEnv.Fill(Engine::Sphere(Engine::Coordinate(0.5, 0.5, 0.5), 0.5), fillAll, Material::water, 273);
    EXPECT_EQ(&Material::water, quadEnv.GetMaterial(Engine::Coordinate(0, 0, 0)));
    EXPECT_EQ(&Material::vacuum, quadEnv.GetMaterial(Engine::Coordinate(1, 0, 0)));
    EXPECT_EQ(&Material::vacuum, quadEnv.GetMaterial(Engine::Coordinate(0, 1, 0)));
    EXPECT_EQ(&Material::vacuum, quadEnv.GetMaterial(Engine::Coordinate(0, 0, 1)));
    EXPECT_DOUBLE_EQ(1.0, quadEnv.Measure(&Material::water));
}

TEST(DiscreteGrid, FillAmount)
{
    DiscreteGrid oneEnv(Engine::Vector(1, 1, 1));
    oneEnv.Fill(Engine::Sphere(Engine::Coordinate(0, 0, 0), 1), fillAll, Material::water, 273, Material::water.normalDensity/2);
    EXPECT_NEAR(0.5, oneEnv.Measure(&Material::water), 0.1);
}

TEST(DiscreteGrid, FillFilter)
{
    DiscreteGrid oneEnv(Engine::Vector(1, 1, 1));
    Filter none = [](const Engine::Coordinate&, const Material*, double, double) { return false;  };
    oneEnv.Fill(Engine::Sphere(Engine::Coordinate(0, 0, 0), 1), none, Material::water, 273);
    EXPECT_EQ(0, oneEnv.Measure(&Material::water));
}



}
}

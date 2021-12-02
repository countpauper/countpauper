#include "pch.h"
#include "Physics/StaticEnvironment.h"
#include "Engine/Volume.h"

namespace Physics
{
namespace Test
{

TEST(Statics, Space)
{
    StaticEnvironment nullEnv(Engine::Vector(0, 0, 0));
    EXPECT_EQ(nullptr, nullEnv.GetMaterial(Engine::Coordinate(0, 0, 0)));

    StaticEnvironment oneEnv(Engine::Vector(1, 1, 1));
    EXPECT_EQ(&Material::vacuum, oneEnv.GetMaterial(Engine::Coordinate(0, 0, 0)));
}

TEST(Statics, Fill)
{
    StaticEnvironment nullEnv(Engine::Vector(0, 0, 0));
    nullEnv.Fill(Engine::Sphere(Engine::Coordinate(0, 0, 0), 1), Material::air, Material::air.normalDensity, 0);
    EXPECT_EQ(nullptr, nullEnv.GetMaterial(Engine::Coordinate(0, 0, 0)));

    StaticEnvironment oneEnv(Engine::Vector(1, 1, 1));
    oneEnv.Fill(Engine::Sphere(Engine::Coordinate(0, 0, 0), 1), Material::water, Material::water.normalDensity, 273);
    EXPECT_EQ(&Material::water, oneEnv.GetMaterial(Engine::Coordinate(0, 0, 0)));
}


}
}

#include "pch.h"
#include "Physics/TreeGrid.h"
#include "Engine/Volume.h"

namespace Physics
{
namespace Test
{

TEST(Tree, Space)
{
    TreeGrid nullEnv(Engine::Vector(0, 0, 0));
    EXPECT_EQ(nullptr, nullEnv.GetMaterial(Engine::Coordinate(0, 0, 0)));

    TreeGrid oneEnv(Engine::Vector(1, 1, 1));
    EXPECT_EQ(&Material::vacuum, oneEnv.GetMaterial(Engine::Coordinate(0, 0, 0)));
}

TEST(Tree, Fill)
{
    TreeGrid nullEnv(Engine::Vector(0, 0, 0));
    nullEnv.Fill(Engine::Sphere(Engine::Coordinate(0, 0, 0), 1), Material::air, 0);
    EXPECT_EQ(nullptr, nullEnv.GetMaterial(Engine::Coordinate(0, 0, 0)));

    TreeGrid oneEnv(Engine::Vector(1, 1, 1));
    oneEnv.Fill(Engine::Sphere(Engine::Coordinate(0, 0, 0), 1), Material::water, 273);
    EXPECT_EQ(&Material::water, oneEnv.GetMaterial(Engine::Coordinate(0, 0, 0)));
    EXPECT_NEAR(273.0, oneEnv.Temperature(Engine::AABox(Engine::Coordinate(0,0,0),Engine::Coordinate(1,1,1))),1);

    TreeGrid quadEnv(Engine::Vector(2, 2, 2));
    quadEnv.Fill(Engine::Sphere(Engine::Coordinate(0.5, 0.5, 0.5), 0.5), Material::water, 273);
    EXPECT_EQ(&Material::water, quadEnv.GetMaterial(Engine::Coordinate(0, 0, 0)));
    EXPECT_EQ(&Material::vacuum, quadEnv.GetMaterial(Engine::Coordinate(1, 0, 0)));
    EXPECT_EQ(&Material::vacuum, quadEnv.GetMaterial(Engine::Coordinate(0, 1, 0)));
    EXPECT_EQ(&Material::vacuum, quadEnv.GetMaterial(Engine::Coordinate(0, 0, 1)));
}


}
}

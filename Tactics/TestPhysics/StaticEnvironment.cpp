#include "pch.h"
#include "Physics/StaticEnvironment.h"

// TODO: TestPhysics

namespace Game
{
namespace Test
{

TEST(Statics, Space)
{
    StaticEnvironment nullEnv(Size(0, 0, 0));
    EXPECT_EQ(nullptr, nullEnv.GetMaterial(Engine::Coordinate(0, 0, 0)));

    StaticEnvironment oneEnv(Size(1, 1, 1));
    EXPECT_EQ(&Material::air, oneEnv.GetMaterial(Engine::Coordinate(0, 0, 0)));
}

TEST(Statics, Fill)
{
    StaticEnvironment nullEnv(Size(0, 0, 0));
    nullEnv.Fill(Engine::Sphere(Engine::Coordinate(0, 0, 0), 1),Material::air);
    EXPECT_EQ(nullptr, nullEnv.GetMaterial(Engine::Coordinate(0, 0, 0)));

    StaticEnvironment oneEnv(Size(1, 1, 1));
    oneEnv.Fill(Engine::Sphere(Engine::Coordinate(0, 0, 0), 1), Material::water);
    EXPECT_EQ(&Material::water, oneEnv.GetMaterial(Engine::Coordinate(0, 0, 0)));
}


}
}

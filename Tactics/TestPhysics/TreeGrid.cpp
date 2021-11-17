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
    EXPECT_DOUBLE_EQ(0, nullEnv.Measure(&Material::vacuum));

    TreeGrid oneEnv(Engine::Vector(1, 1, 1));
    EXPECT_EQ(&Material::vacuum, oneEnv.GetMaterial(Engine::Coordinate(0, 0, 0)));
    EXPECT_DOUBLE_EQ(1.0, oneEnv.Measure(&Material::vacuum));
    EXPECT_EQ(nullptr, oneEnv.GetMaterial(Engine::Coordinate(-1, 0, 0)));
    EXPECT_EQ(nullptr, oneEnv.GetMaterial(Engine::Coordinate(0, 0, 2)));

    TreeGrid splitEnv(Engine::Vector(2, 2, 2));
    EXPECT_EQ(nullptr, splitEnv.GetMaterial(Engine::Coordinate(-1, 0, 0)));
    EXPECT_EQ(nullptr, splitEnv.GetMaterial(Engine::Coordinate( 0, 3, 0)));
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
    EXPECT_DOUBLE_EQ(1.0, oneEnv.Measure(&Material::water));
    EXPECT_DOUBLE_EQ(0.0, oneEnv.Measure(&Material::vacuum));

    TreeGrid quadEnv(Engine::Vector(2, 2, 2));
    EXPECT_DOUBLE_EQ(2*2*2, quadEnv.Measure(&Material::vacuum));
    quadEnv.Fill(Engine::Sphere(Engine::Coordinate(0.5, 0.5, 0.5), 0.5), Material::water, 273);
    EXPECT_EQ(&Material::water, quadEnv.GetMaterial(Engine::Coordinate(0, 0, 0)));
    EXPECT_EQ(&Material::vacuum, quadEnv.GetMaterial(Engine::Coordinate(1, 0, 0)));
    EXPECT_EQ(&Material::vacuum, quadEnv.GetMaterial(Engine::Coordinate(0, 1, 0)));
    EXPECT_EQ(&Material::vacuum, quadEnv.GetMaterial(Engine::Coordinate(0, 0, 1)));
    EXPECT_DOUBLE_EQ(1.0, quadEnv.Measure(&Material::water));
}

TEST(Tree, FillOne)
{
    Grid grid;
    Physics::Box bounds(Physics::Size(4, 4, 4));
    for (BoxIterator i(bounds); i != i.end(); ++i)
    {
        TreeGrid env(grid.Meters(bounds.Extent()), grid);
        Engine::AABox volume(Engine::Coordinate(i->x, i->y, i->z), Engine::Vector(1, 1, 1));
        EXPECT_EQ(1, env.Fill(volume, Material::water,300));
        EXPECT_EQ(&Material::water, env.GetMaterial(Engine::Coordinate(i->x + 0.5, i->y + 0.5, i->z + 0.5)));
        EXPECT_DOUBLE_EQ(grid.Volume(), env.Measure(&Material::water));
        EXPECT_DOUBLE_EQ(((4*4*4)-1)* grid.Volume(), env.Measure(&Material::vacuum));
    }
}

TEST(Tree, FillOneByOne)
{
    Grid grid;
    Physics::Box bounds(Physics::Size(4, 4, 4));
    TreeGrid env(grid.Meters(bounds.Extent()), grid);
    double expectedVolume = 0;
    for (BoxIterator i(bounds); i != i.end(); ++i)
    {
        Engine::AABox volume(Engine::Coordinate(i->x, i->y, i->z), Engine::Vector(1, 1, 1));
        EXPECT_EQ(1, env.Fill(volume, Material::water, 300));
        EXPECT_EQ(&Material::water, env.GetMaterial(Engine::Coordinate(i->x + 0.5, i->y + 0.5, i->z + 0.5)));
        expectedVolume += grid.Volume();
        EXPECT_DOUBLE_EQ(expectedVolume, env.Measure(&Material::water));
        EXPECT_DOUBLE_EQ(4 * 4 * 4 * grid.Volume() - expectedVolume, env.Measure(&Material::vacuum));
    }
}

}
}
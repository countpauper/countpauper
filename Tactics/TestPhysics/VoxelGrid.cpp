#include "pch.h"
#include "Physics/VoxelGrid.h"
#include "Engine/Volume.h"

namespace Physics
{

    class TestVoxel
    {
    public:
        TestVoxel() : m_material(&Material::vacuum) {}

        TestVoxel(const Material& m, double temperature, double fraction) :
            m_material(&m),
            m_temperature(temperature),
            m_fraction(fraction)
        {
        }

        const Material* GetMaterial() const
        {
            return m_material;
        }
        double Temperature() const
        {
            return m_temperature;
        }
        double Measure(const Material* m) const
        {
            if (m == GetMaterial())
                return m_fraction;
            else
                return 0.0;
        }
        Engine::RGBA Color() const
        {
            return m_material->color;
        }
        double Density() const
        {
            return m_material->normalDensity;
        }
    private:
        const Material* m_material;
        double m_temperature = 0;
        double m_fraction = 1.0;
    };

    class TestGrid : public VoxelGrid<TestVoxel>
    {
    public:
        TestGrid(const Engine::Vector& size) :
            VoxelGrid(size)
        {
        }
    private:
         void Tick(double) {}
    };

namespace Test
{
TEST(VoxelGrid, Space)
{
    TestGrid nullEnv(Engine::Vector(0, 0, 0));
    EXPECT_EQ(nullptr, nullEnv.GetMaterial(Engine::Coordinate::origin));
    EXPECT_DOUBLE_EQ(0, nullEnv.Measure(&Material::vacuum));

    TestGrid oneEnv(Engine::Vector(1, 1, 1));
    EXPECT_EQ(&Material::vacuum, oneEnv.GetMaterial(Engine::Coordinate::origin));
    EXPECT_DOUBLE_EQ(1.0, oneEnv.Measure(&Material::vacuum));
    EXPECT_EQ(nullptr, oneEnv.GetMaterial(Engine::Coordinate(-1, 0, 0)));
    EXPECT_EQ(nullptr, oneEnv.GetMaterial(Engine::Coordinate(0, 0, 2)));

    TestGrid splitEnv(Engine::Vector(2, 2, 2));
    EXPECT_EQ(nullptr, splitEnv.GetMaterial(Engine::Coordinate(-1, 0, 0)));
    EXPECT_EQ(nullptr, splitEnv.GetMaterial(Engine::Coordinate(0, 3, 0)));
}

TEST(VoxelGrid, Fill)
{
    TestGrid nullEnv(Engine::Vector(0, 0, 0));
    nullEnv.Fill(Engine::Sphere(Engine::Coordinate::origin, 1), fillAll, Physics::Material::air, 0);

    EXPECT_EQ(nullptr, nullEnv.GetMaterial(Engine::Coordinate::origin));

    TestGrid oneEnv(Engine::Vector(1, 1, 1));
    oneEnv.Fill(Engine::Sphere(Engine::Coordinate::origin, 1), fillAll, Material::water, 273);
    EXPECT_EQ(&Material::water, oneEnv.GetMaterial(Engine::Coordinate::origin));
    EXPECT_NEAR(273.0, oneEnv.Temperature(Engine::AABB(Engine::Coordinate::origin, Engine::Coordinate(1, 1, 1))), 1);
    EXPECT_DOUBLE_EQ(1.0, oneEnv.Measure(&Material::water));
    EXPECT_DOUBLE_EQ(0.0, oneEnv.Measure(&Material::vacuum));

    TestGrid quadEnv(Engine::Vector(2, 2, 2));
    EXPECT_DOUBLE_EQ(2 * 2 * 2, quadEnv.Measure(&Material::vacuum));
    quadEnv.Fill(Engine::Sphere(Engine::Coordinate(0.5, 0.5, 0.5), 0.5), fillAll, Material::water, 273);
    EXPECT_EQ(&Material::water, quadEnv.GetMaterial(Engine::Coordinate::origin));
    EXPECT_EQ(&Material::vacuum, quadEnv.GetMaterial(Engine::Coordinate(1, 0, 0)));
    EXPECT_EQ(&Material::vacuum, quadEnv.GetMaterial(Engine::Coordinate(0, 1, 0)));
    EXPECT_EQ(&Material::vacuum, quadEnv.GetMaterial(Engine::Coordinate(0, 0, 1)));
    EXPECT_DOUBLE_EQ(1.0, quadEnv.Measure(&Material::water));
}

TEST(VoxelGrid, FillAmount)
{
    TestGrid oneEnv(Engine::Vector(1, 1, 1));
    oneEnv.Fill(Engine::Sphere(Engine::Coordinate::origin, 1), fillAll, Material::water, 273, Material::water.normalDensity/2);
    EXPECT_NEAR(0.5, oneEnv.Measure(&Material::water), 0.1);
}

TEST(VoxelGrid, FillFilter)
{
    TestGrid oneEnv(Engine::Vector(1, 1, 1));
    Filter none = [](const Engine::Coordinate&, const Material*, double, double) { return false;  };
    oneEnv.Fill(Engine::Sphere(Engine::Coordinate::origin, 1), none, Material::water, 273);
    EXPECT_EQ(0, oneEnv.Measure(&Material::water));
}

TEST(VoxelGrid, Statistics)
{
    TestGrid oneEnv(Engine::Vector(1, 1, 1));
    EXPECT_EQ(oneEnv.Statistics(), L"0kB");
    TestGrid bigEnv(Engine::Vector(10, 5, 5));
    EXPECT_EQ(bigEnv.Statistics(), L"5kB");
}

}
}

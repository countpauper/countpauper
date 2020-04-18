#include "stdafx.h"
#include "VoxelMap.h"


namespace Game
{


    //                                              Name            Melt    Boil    Density Conduct.    Transparency
    VoxelMap::Material VoxelMap::Material::vacuum   { L"Vacuum",    0,      0,      0,      0,          1.0 };
    VoxelMap::Material VoxelMap::Material::air      { L"Air",       60,     80,     1.3,    29.2/29.0,  0.99 };    // mix of N2 and O2, 29 g/mol
    VoxelMap::Material VoxelMap::Material::soil     { L"Soil",      0,      0,      1600,   0.4/42.0,   0.0 };     // 25% water 25% air, 50% solid = 42 g/mol
    VoxelMap::Material VoxelMap::Material::stone    { L"Stone",     1986,   3220,   2648,   10/60,      0.0 };     // for now 100% silicon dioxide, 60 g/mol
    VoxelMap::Material VoxelMap::Material::water    { L"Water",     273,    373,    1000,   0.6065/18,  0.5 };     // H2O 18 g/mol
    // redundant for now VoxelMap::Material VoxelMap::Material::clay  { L"Clay",      0,      0,      1700,        2.9,           0.0           };     // 
    // redundant for now VoxelMap::Material VoxelMap::Material::sand  { L"Sand",      1986,   3220,   2648,        10,            0.0           };     // silicon dioxide, 60 g/mol


    VoxelMap::VoxelMap() :
        x(0),
        y(0),
        z(0),
        airTemperature(0),
        airPressure(0)
    {
    }

    VoxelMap::Voxel::Voxel() :
        material(VoxelMap::Material::vacuum),
        temperature(0),
        pressure(0),
        flow(0, 0, 0),
        fixed(false)
    {
    }


    void VoxelMap::Space(unsigned x, unsigned y, unsigned z)
    {
        this->x = x;
        this->y = y;
        this->z = z;
        voxels.resize(this->x * this->y * this->z);
    }

    void VoxelMap::Air(double temperature, double pressure)
    {
        airTemperature = temperature;
        airPressure = pressure;
        for (auto& voxel : voxels)
        {
            if (&voxel.material == &Material::vacuum)
            {
                voxel.material = Material::air;
                voxel.pressure = airPressure;
                voxel.temperature = airTemperature;
            }
        }
    }

    void VoxelMap::Rock(int minRock, double stddev, double gravity)
    {

    }

    unsigned VoxelMap::Latitude() const
    {
        return x;
    }
    unsigned VoxelMap::Longitude() const
    {
        return y;
    }
    unsigned VoxelMap::Altitude() const
    {
        return z;
    }



    Square VoxelMap::At(const Position& p) const
    {
        return Square();
    }

    void VoxelMap::Render() const
    {

    }

}
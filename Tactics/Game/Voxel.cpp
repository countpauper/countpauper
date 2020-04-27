#include "stdafx.h"
#include "VoxelMap.h"
#include <gl/GL.h>
#include "Engine/Geometry.h"
#include "Engine/Drawing.h"
#include "Engine/Text.h"
#include <string>

namespace Game
{
const VoxelMap::Material VoxelMap::Material::vacuum{ L"Vacuum",    Element::None,    0xFF000000,   0,      0,      0,      0,            0,          0,           0,         0,            0.0 };
const VoxelMap::Material VoxelMap::Material::air{ L"Air",       Element::Air,     0xFFA08040,   60,     80,     1.225,  29,           29.2,       10e-3,       1e9,       1,            0.01 };     // mix of N2 and O2, 29 g/mol
const VoxelMap::Material VoxelMap::Material::soil{ L"Soil",      Element::Nature,  0xFF20FF20,   0,      0,      1600,   65,           0.4,        0.7,         1e7,       100,          10 };     // 65g/mol, based on 0% humidity. Part SiO2, N2 and proteins
const VoxelMap::Material VoxelMap::Material::stone{ L"Stone",     Element::Stone,   0xFFA0A0A0,   1986,   3220,   2648,   60,           10,         0.8,         1e10,      1,            10 };     // for now 100% silicon dioxide, 60 g/mol
const VoxelMap::Material VoxelMap::Material::water{ L"Water",     Element::Water,   0xFFFF6010,   273,    373,    1000,   18,           0.6065,     0.07,        9e9,       1,            6.9 / 1000 };     // H2O 18 g/mol
                                                                                                                                                                                                            // redundant for now VoxelMap::Material VoxelMap::Material::sand  { L"Sand",      1986,   3220,   2648,        10,            0.0           };     // silicon dioxide, 60 g/mol

VoxelMap::Voxel::Voxel() :
    material(&VoxelMap::Material::vacuum),
    temperature(0),
    mass(0),
    flow(0, 0, 0),
    fixed(false),
    boundary(false)
{
}

bool VoxelMap::Voxel::Solid() const
{
    return temperature < material->melt;
}

bool VoxelMap::Voxel::Gas() const
{
    return temperature > material->boil;
}

Square VoxelMap::Voxel::Square(unsigned height) const
{
    if (temperature >= 500.0f)
    {
        return Game::Square(Element::Fire, Solid(), height);
    }
    else
    {
        return Game::Square(material->element, Solid(), height);
    }
}

double VoxelMap::Voxel::Translucency() const
{
    // https://en.wikipedia.org/wiki/Opacity_(optics)#Quantitative_definition
    // assume light from straight above (that's why it's vertical el)
    return exp(-VerticalEl * MeterPerEl * material->opacity * Density());
}

bool VoxelMap::Voxel::Opaque() const
{
    return Translucency() < 0.01;
}

bool VoxelMap::Voxel::Transparent() const
{
    return Translucency() > 0.99;
}

double VoxelMap::Voxel::Density() const
{
    return mass / LiterPerBlock;
}

double VoxelMap::Voxel::Molecules() const
{
    return mass / material->molarMass;
}

double VoxelMap::Voxel::Harndess() const
{
    if (Gas())
        return 0;
    else if (Solid())
        return material->youngsModulus;
    else
        return material->surfaceTension;
}
double VoxelMap::Voxel::Pressure() const
{   // Pascal = newton /m2 = kg/(m*s^2)  = 1 J/m^3 
    // ideal gas law (for gas)
    constexpr double IdealGas = 0.08206 * PascalPerAtmosphere; // ideal gas constant in atmosphere * Pascal/Atmosphere
    return (Molecules() * IdealGas * temperature) / Volume();
}


double VoxelMap::Voxel::Volume() const
{
    return MeterPerEl * HorizontalEl *
        MeterPerEl* HorizontalEl *
        MeterPerEl * VerticalEl *
        1000.0;
}

Engine::RGBA VoxelMap::Voxel::Color() const
{
    // TODO: why is steam white, water blue, something about particle size and scattering
    // Also solids need to start glowing red when hot 
    auto color = material->color.Translucent(1.0 - Translucency());

    return color;
}



}
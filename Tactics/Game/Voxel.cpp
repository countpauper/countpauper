#include "stdafx.h"
#include "VoxelMap.h"
#include <gl/GL.h>
#include "Engine/Maths.h"
#include "Engine/Drawing.h"
#include "Engine/Text.h"
#include <string>

namespace Game

{
//                                                  Name,           Element,        Color,         Melt,   Boil, Density, molar mass,   Viscosity,  Conduct,    Capacity    surfaceTensiom, youngModulus,  Opacity,         
const VoxelMap::Material VoxelMap::Material::vacuum{ L"Vacuum",    Element::None,    0xFF000000,   0,      0,      0,      0,           0,          0,         0,          0,              0,             0.0          };
const VoxelMap::Material VoxelMap::Material::air   { L"Air",       Element::Air,     0xFFA08040,   60,     80,    1.225,  29,           18e-6,      29.2,       1.012,      10e-3,          1e9,           0.01         };     // 28.964g/mol because n2&o2 diatomic
const VoxelMap::Material VoxelMap::Material::soil  { L"Soil",      Element::Nature,  0xFF20FF20,   0,      0,     1600,   65,           10e3,       0.4,        2.0,        0.7,            1e7,           10           };     // 65g/mol, based on 0% humidity. Part SiO2, N2 and proteins
const VoxelMap::Material VoxelMap::Material::stone { L"Stone",     Element::Stone,   0xFFA0A0A0,   1986,   3220,   2648,   60,          10e21,      10,         0.790,      0.8,            1e10,          10           };     // for now 100% silicon dioxide, 60 g/mol
const VoxelMap::Material VoxelMap::Material::water { L"Water",     Element::Water,   0xFFFF6010,   273,    373,    1000,   18,          8.9e-4,     0.6065,     4.1813,     0.07,           9e9,           6.9 / 1000   };     // H2O 18 g/mol
                                                                                                                                                                                                            // redundant for now VoxelMap::Material VoxelMap::Material::sand  { L"Sand",      1986,   3220,   2648,        10,            0.0           };     // silicon dioxide, 60 g/mol

bool VoxelMap::Voxel::Solid() const
{
    return temperature < material.melt;
}

bool VoxelMap::Voxel::Gas() const
{
    return temperature > material.boil;
}

Square VoxelMap::Voxel::Square(unsigned height) const
{
    if (temperature >= 500.0f)
    {
        return Game::Square(Element::Fire, Solid(), height);
    }
    else
    {
        return Game::Square(material.element, Solid(), height);
    }
}

double VoxelMap::Voxel::Translucency() const
{
    // https://en.wikipedia.org/wiki/Opacity_(optics)#Quantitative_definition
    // assume light from straight above (that's why it's vertical el)
    return exp(-VerticalEl * MeterPerEl * material.opacity * Density());
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
    return density;
}

double VoxelMap::Voxel::Mass() const
{
    return density * LiterPerBlock;
}

double VoxelMap::Voxel::Molecules() const
{
    return Mass() / material.molarMass;
}

double VoxelMap::Voxel::Viscosity() const
{
    if (Solid())
    {   // TODO: better formula based on difference with melting point and viscosity multiplier 
        return 10e10;
    }
    else if (Gas())
    {   // TODO: better formula based on difference     with boiling point
        //  and viscosity multiplier
        return 1e-6;
    }
    else
    {
        //  specific for water https://en.wikipedia.org/wiki/Viscosity#Dynamic_and_kinematic_viscosity
        //   TODO: use a viscosity multiplier
        return 0.02939 * exp(507.88 / (temperature - 149.3)); 
    }
}

double VoxelMap::Voxel::Hardness() const
{
    if (Gas())
        return 0;
    else if (Solid())
        return material.youngsModulus;
    else
        return material.surfaceTension;
}
double VoxelMap::Voxel::Pressure() const
{   // Pascal = newton /m2 = kg/(m*s^2)  = 1 J/m^3 
    // ideal gas law (for gas)  https://en.wikipedia.org/wiki/Ideal_gas_law
    return (Molecules() * IdealGasConstant * temperature) / Volume();
}

double VoxelMap::Voxel::DiffusionCoefficient(const VoxelMap::Voxel& to) const
{   //  TODO: physics https://en.wikipedia.org/wiki/Mass_diffusivity;
    //  especially when each voxel can contain multiple materials
    if (Solid())
        return 0;
    if (to.Solid())
        return 0;
    if (Gas() && to.Gas())
    {
        constexpr double DiffusionCoefficient = 1.859e-3; // * 1e4 / PascalPerAtmosphere; // in m2 pascal 
        constexpr double AverageCollisionDiameter = 300e-12;    // TODO: table per material, is lacking SiO https://en.wikipedia.org/wiki/Kinetic_diameter
//        return DiffusionCoefficient * pow(temperature, 1.5) * sqrt(1 / material->molarMass + 1 / to.material->molarMass) / (Pressure()/PascalPerAtmosphere) *
//            Engine::Average(AverageCollisionDiameter, ApproximateCollisionDiameter) * temperatureIndependentCollisionIntegral; // 
        // based on calibrated air = 0.176cm2/sec at 1 atmosphere 25 C
        // doesn't take into account collision coefficient or diameter
        // http://www.thermopedia.com/content/696/
        constexpr double DiffusionConstant = 1.2e-4 * PascalPerAtmosphere;

        return DiffusionConstant * pow(temperature, 1.5) * sqrt(1 / material.molarMass + 1 / to.material.molarMass) / Pressure();
    }
    else 
        return 0.01;
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
    auto color = material.color.Translucent(1.0 - Translucency());

    return color;
}

void VoxelMap::Voxel::Render(const Position& p, const Directions& visibility, bool analysis) const
{
    auto c = Color();
    c.Render();
    unsigned mode = analysis ? GL_LINE_LOOP : GL_QUADS;
    if (visibility[Direction::south])
    {
        glPushName(LocationName(p, Direction::south));
        glNormal3d(0, 0, -1); //  Direction::south.Vector()
        glBegin(mode);
        glVertex3d(0, 0 * MeterPerEl, 0);
        glVertex3d(0, 1 * MeterPerEl, 0);
        glVertex3d(1, 1 * MeterPerEl, 0);
        glVertex3d(1, 0 * MeterPerEl, 0);
        glEnd();
        glPopName();
    }
    // north
    if (visibility[Direction::north])
    {
        glPushName(LocationName(p, Direction::north));
        glNormal3d(0, 0, 1);
        glBegin(mode);
        glVertex3d(0, 0 * MeterPerEl, 1);
        glVertex3d(1, 0 * MeterPerEl, 1);
        glVertex3d(1, 1 * MeterPerEl, 1);
        glVertex3d(0, 1 * MeterPerEl, 1);
        glEnd();
        glPopName();
    }
    // east
    if (visibility[Direction::east])
    {
        glPushName(LocationName(p, Direction::east));
        glNormal3d(1, 0, 0);
        glBegin(mode);
        glVertex3d(1, 0 * MeterPerEl, 0);
        glVertex3d(1, 1 * MeterPerEl, 0);
        glVertex3d(1, 1 * MeterPerEl, 1);
        glVertex3d(1, 0 * MeterPerEl, 1);
        glEnd();
        glPopName();
    }
    // west
    if (visibility[Direction::west])
    {
        glPushName(LocationName(p, Direction::west));
        glNormal3d(-1, 0, 0);
        glBegin(mode);
        glVertex3d(0, 0 * MeterPerEl, 0);
        glVertex3d(0, 0 * MeterPerEl, 1);
        glVertex3d(0, 1 * MeterPerEl, 1);
        glVertex3d(0, 1 * MeterPerEl, 0);
        glEnd();
        glPopName();
    }
    // top
    if (visibility[Direction::up])
    {
        glPushName(LocationName(p, Direction::up));
        glNormal3d(0, 1 * MeterPerEl, 0);
        glBegin(mode);
        glVertex3d(0, 1 * MeterPerEl, 0);
        glVertex3d(0, 1 * MeterPerEl, 1);
        glVertex3d(1, 1 * MeterPerEl, 1);
        glVertex3d(1, 1 * MeterPerEl, 0);
        glEnd();
        glPopName();
    }

    if (analysis)
    {
        glColor3d(1, 0, 0);
        glPushMatrix();
        glTranslated(0.25, 0.25* MeterPerEl, 0.25);
        Engine::glText(std::to_string(Pressure() / PascalPerAtmosphere));
        // Engine::Vector flowArrow(flow.x, flow.z, flow.y);
        //double length = 0.5 - (0.5 / flowArrow.Length());
        //flowArrow = flowArrow.Normal()*length;
        // glDrawArrow(flowArrow);
        glPopMatrix();
    }
}




}
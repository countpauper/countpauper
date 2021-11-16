#include "stdafx.h"
#include "Voxel.h"
#include "Engine/Maths.h"
#include "Engine/Drawing.h"
#include "Engine/Text.h"
#include "Engine/Mesh.h"
#include "Engine/Matrix.h"
#include <string>

namespace Physics
{
//                               Name,          Color,                      Melt,   Boil, Density, molar mass,   Viscosity,  Conduct,    Capacity    surfaceTensiom, youngModulus,  Opacity,         
const Material Material::vacuum { L"Vacuum",    Engine::RGBA(0x00000000),   0,      0,      0,      0,           0,          0,         0,          0,              0,             0.0          };
const Material Material::air   { L"Air",       Engine::RGBA(0xFFA08040),   60,     80,    1.225,  29,           18e-6,      29.2,       1.012,      10e-3,          1e9,           0.01         };     // 28.964g/mol because n2&o2 diatomic
const Material Material::soil  { L"Soil",      Engine::RGBA(0xFF20FF20),   0,      0,     1600,   65,           10e3,       0.4,        2.0,        0.7,            1e7,           10           };     // 65g/mol, based on 0% humidity. Part SiO2, N2 and proteins
const Material Material::stone { L"Stone",     Engine::RGBA(0xFFA0A0A0),   1986,   3220,   2648,   60,          10e21,      10,         0.790,      0.8,            1e10,          10           };     // for now 100% silicon dioxide, 60 g/mol
const Material Material::water { L"Water",     Engine::RGBA(0xFFFF6010),   273,    373,    1000,   18,          8.9e-4,     0.6065,     4.1813,     0.07,           9e9,           6.9 / 1000   };     // H2O 18 g/mol
                                                                                                                                                                                                            // redundant for now VoxelMap::Material VoxelMap::Material::sand  { L"Sand",      1986,   3220,   2648,        10,            0.0           };     // silicon dioxide, 60 g/mol

bool Voxel::Solid() const
{
    return temperature < material->melt;
}

bool Voxel::Gas() const
{
    return temperature > material->boil;
}

double Voxel::Translucency(double distance) const
{
    // https://en.wikipedia.org/wiki/Opacity_(optics)#Quantitative_definition
    // assume light from straight above (that's why it's vertical el)
    return exp(-distance * material->opacity * Density());
}

bool Voxel::Opaque(double distance) const
{
    return Translucency(distance) < 0.01;
}

bool Voxel::Transparent(double distance) const
{
    return Translucency(distance) > 0.99;
}

double Voxel::Density() const
{
    return density;
}

double Voxel::Mass(double volume) const
{
    return density * volume;
}
/*
double Voxel::Molecules(double volume) const
{
    return Mass(volume) / material->molarMass;
}
*/

double Voxel::Viscosity() const
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

double Voxel::Hardness() const
{
    if (Gas())
        return 0;
    else if (Solid())
        return material->youngsModulus;
    else
        return material->surfaceTension;
}
double Voxel::Pressure() const
{   // Pascal = newton /m2 = kg/(m*s^2)  = 1 J/m^3 
    // ideal gas law (for gas)  https://en.wikipedia.org/wiki/Ideal_gas_law
    // mass = density * volume and later divided by volume again, can compute without volume from density
    // return (Molecules(Volume()) * IdealGasConstant * temperature) / Volume();
    return density / material->molarMass * IdealGasConstant * temperature;
}

double Voxel::DiffusionCoefficient(const Voxel& to) const
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

        return DiffusionConstant * pow(temperature, 1.5) * sqrt(1 / material->molarMass + 1 / to.material->molarMass) / Pressure();
    }
    else 
        return 0.01;
}

Engine::RGBA Voxel::Color(double distance) const
{
    // TODO: why is steam white, water blue, something about particle size and scattering
    // Also solids need to start glowing red when hot 
    auto color = material->color.Translucent(1.0 - Translucency(distance));

    return color;
}
/*
void Voxel::RenderAnalysis(const Position& p, const Directions& visibility, const Engine::Vector& densityGradient) const
{
    glColor3ub(200, 200, 200);
    glPushMatrix();
        glTranslated(0.25, 0.25* MeterPerEl, 0.25);
        Engine::glText(std::to_string(Pressure() / PascalPerAtmosphere));
        // Engine::Vector flowArrow(flow.x, flow.z, flow.y);
        //double length = 0.5 - (0.5 / flowArrow.Length());
        //flowArrow = flowArrow.Normal()*length;
        // glDrawArrow(flowArrow);
    glPopMatrix();

    double offDensity = density - material->normalDensity;
    double sigmoidDensity = Engine::Sigmoid(offDensity);
    glColor3d(0, sigmoidDensity, 1 - sigmoidDensity);
    RenderFaces(p, visibility, GL_LINE_LOOP);
}


void Voxel::Render(const Position& p, const Directions& visibility) const
{
    auto c = Color();
    c.Render();
    RenderFaces(p, visibility, GL_QUADS);

}

void Voxel::RenderFaces(const Position& p, const Directions& visibility, unsigned mode) const
{
    for (auto direction : visibility)
    {
        RenderFace(p, direction, mode);
    }
}

void Voxel::RenderFace(const Position& p, Direction direction, unsigned mode) const
{
    glPushName(Map::LocationName(p, direction));

    glNormal3d(direction.Vector().x, direction.Vector().z, direction.Vector().y);

    // TODO vertex buffers
    if (direction == Direction::south)
    {
        glBegin(mode);
            glVertex3d(0, 0 * MeterPerEl, 0);
            glVertex3d(0, 1 * MeterPerEl, 0);
            glVertex3d(1, 1 * MeterPerEl, 0);
            glVertex3d(1, 0 * MeterPerEl, 0);
        glEnd();
    }
    // north
    else if (direction == Direction::north)
    {
        glBegin(mode);
        glVertex3d(0, 0 * MeterPerEl, 1);
        glVertex3d(1, 0 * MeterPerEl, 1);
        glVertex3d(1, 1 * MeterPerEl, 1);
        glVertex3d(0, 1 * MeterPerEl, 1);
        glEnd();
    }
    // east
    else if (direction == Direction::east)
    {
        glBegin(mode);
        glVertex3d(1, 0 * MeterPerEl, 0);
        glVertex3d(1, 1 * MeterPerEl, 0);
        glVertex3d(1, 1 * MeterPerEl, 1);
        glVertex3d(1, 0 * MeterPerEl, 1);
        glEnd();
    }
    // west
    else if (direction == Direction::west)
    {
        glBegin(mode);
        glVertex3d(0, 0 * MeterPerEl, 0);
        glVertex3d(0, 0 * MeterPerEl, 1);
        glVertex3d(0, 1 * MeterPerEl, 1);
        glVertex3d(0, 1 * MeterPerEl, 0);
        glEnd();
    }
    // top
    if (direction == Direction::up)
    {
        glBegin(mode);
        glVertex3d(0, 1 * MeterPerEl, 0);
        glVertex3d(0, 1 * MeterPerEl, 1);
        glVertex3d(1, 1 * MeterPerEl, 1);
        glVertex3d(1, 1 * MeterPerEl, 0);
        glEnd();
    }
    glPopName();
}
*/



}
#include "stdafx.h"
#include "VoxelMap.h"
#include <gl/GL.h>
#include "Engine/Image.h"
#include "Engine/Error.h"
#include <string>

namespace Game
{


//                                                      Name          Element           Color         Melt    Boil    Density Conduct.    Opacity
const VoxelMap::Material VoxelMap::Material::vacuum   { L"Vacuum",    Element::None,    0xFF000000,   0,      0,      0,      0,          0.0        };
const VoxelMap::Material VoxelMap::Material::air      { L"Air",       Element::Air,     0xFFFF4020,   60,     80,     1.225,  29.2/29.0,  0.01       };    // mix of N2 and O2, 29 g/mol
const VoxelMap::Material VoxelMap::Material::soil     { L"Soil",      Element::Nature,  0xFF20FF20,   0,      0,      1600,   0.4/42.0,   10         };     // 25% water 25% air, 50% solid = 42 g/mol
const VoxelMap::Material VoxelMap::Material::stone    { L"Stone",     Element::Stone,   0xFFA0A0A0,   1986,   3220,   2648,   10/60,      10         };     // for now 100% silicon dioxide, 60 g/mol
const VoxelMap::Material VoxelMap::Material::water    { L"Water",     Element::Water,   0xFF1040FF,   273,    373,    1000,   0.6065/18,  6.9/1000   };     // H2O 18 g/mol
// redundant for now VoxelMap::Material VoxelMap::Material::clay  { L"Clay",      0,      0,      1700,        2.9,           0.0           };     // 
// redundant for now VoxelMap::Material VoxelMap::Material::sand  { L"Sand",      1986,   3220,   2648,        10,            0.0           };     // silicon dioxide, 60 g/mol

bool VoxelMap::Voxel::Solid() const
{
    return temperature < material->melt;
}

bool VoxelMap::Voxel::Gas() const
{
    return temperature > material->boil;
}

Square VoxelMap::Voxel::Square() const
{
    return Game::Square(material->element, Solid());
}

double VoxelMap::Voxel::Translucency() const
{
    // https://en.wikipedia.org/wiki/Opacity_(optics)#Quantitative_definition
    return exp(-VerticalEl * MeterPerEl * material->opacity * Density());
}

bool VoxelMap::Voxel::Opaque() const
{
    return Translucency() < 0.01;
}

bool VoxelMap::Voxel::Transparent() const
{
    return Translucency() < 0.99;
}

double VoxelMap::Voxel::Density() const
{
    return mass / LiterPerBlock;
}

Engine::RGBA VoxelMap::Voxel::Color() const
{
    // TODO: why is steam white, water blue, something about particle size and scattering
    // Also solids need to start glowing red when hot 
    auto color = material->color.Translucent(1.0-Translucency());

    return color;
}

VoxelMap::VoxelMap() :
    x(0),
    y(0),
    z(0),
    airTemperature(0)
{
}

VoxelMap::Voxel::Voxel() :
    material(&VoxelMap::Material::vacuum),
    temperature(0),
    mass(0),
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

void VoxelMap::Air(double temperature)
{
    airTemperature = temperature;
    for (auto& voxel : voxels)
    {
        if (voxel.material == &Material::vacuum)
        {
            voxel.material = &Material::air;
            voxel.mass = voxel.material->normalDensity * LiterPerBlock;
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

const VoxelMap::Voxel& VoxelMap::Get(unsigned x, unsigned y, unsigned z) const
{   // ordered Z,Y,X, in direction of gravity for best caching
    return voxels.at(z + y * this->z + x * this->z*this->y);
}

VoxelMap::Voxel& VoxelMap::Get(unsigned x, unsigned y, unsigned z)
{
    return voxels.at(z + y * this->z + x * this->z*this->y);
}

Square VoxelMap::At(const Position& p) const
{
    if (p.x < 0)
        return Square();
    if (unsigned(p.x) >= Longitude())
        return Square();
    if (p.y < 0)
        return Square();
    if (unsigned(p.y) >= Latitude())
        return Square();
    for (int i = z-1; i >= 0; --i)
    {
        const auto& v = Get(p.x, p.y, i);
        if (!v.Gas())
        {
            Square s = v.Square();
            s.height = i;
            return s;
        }
    }
    return Square(Element::Fire, false, 0); // welcome to hell
}

void VoxelMap::Render() const
{
    unsigned x = 0, y = 0, z = 0;
    for (auto& v : voxels)
    {
        glPushMatrix();
        glTranslatef(float(x), float(z), float(y));

        if (!v.Transparent())
        {
            v.Color().Render();

            glPushName(LocationName(x, y, z, Direction::south));
            glBegin(GL_QUADS);
                // front
                glVertex3f(0, 0, 0);
                glVertex3f(1, 0, 0);
                glVertex3f(1, 1, 0);
                glVertex3f(0, 1, 0);
            glEnd();
            glPopName();


            // back
            glPushName(LocationName(x, y, z, Direction::north));
            glBegin(GL_QUADS);
                glVertex3f(0, 0, 1);
                glVertex3f(1, 0, 1);
                glVertex3f(1, 1, 1);
                glVertex3f(0, 1, 1);
            glEnd();
            glPopName();
            // right
            glPushName(LocationName(x, y, z, Direction::east));
            glBegin(GL_QUADS);
                glVertex3f(1, 0, 0);
                glVertex3f(1, 0, 1);
                glVertex3f(1, 1, 1);
                glVertex3f(1, 1, 0);
            glEnd();
            glPopName();
                // left
            glPushName(LocationName(x, y, z, Direction::west));
            glBegin(GL_QUADS);
                glVertex3f(0, 0, 0);
                glVertex3f(0, 0, 1);
                glVertex3f(0, 1, 1);
                glVertex3f(0, 1, 0);
            glEnd();
            glPopName();
            // top
            glPushName(LocationName(x, y, z, Direction::up));
            glBegin(GL_QUADS);
                glVertex3d(0, 1, 0);
                glVertex3d(1, 1, 0);
                glVertex3d(1, 1, 1);
                glVertex3d(0, 1, 1);
            glEnd();
            glPopName();
        }

        if ((z == 0) && (!v.Opaque()))
        {   // lava bottom of the level
            glPushName(LocationName(x, y, 0, Direction::up));
            glColor4ub(255, 50, 50, 255);
            glBegin(GL_QUADS);
                glVertex3d(0, 1, 0);
                glVertex3d(1, 1, 0);
                glVertex3d(1, 1, 1);
                glVertex3d(0, 1, 1);
            glEnd();
            glPopName();
        }
        if (++z == Altitude())
        {
            z = 0;
            if (++y == Latitude())
            {
                y = 0;
                if (++x == Longitude())
                {
                    x = 0;
                }
            }
        }
        //glPopName();
        glPopMatrix();
    }
    Engine::CheckGLError();
}

std::wistream& operator>>(std::wistream& s, VoxelMap& map)
{
    std::wstring name;
    unsigned longitude, latitude, altitude;
    s >> name >> longitude >> latitude >> altitude;
    map.Space(longitude, latitude, altitude);
    double temperature;
    s >> temperature; 
    map.Air(temperature);
    return s;
}

}
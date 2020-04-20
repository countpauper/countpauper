#include "stdafx.h"
#include "VoxelMap.h"
#include <gl/GL.h>
#include "Engine/Image.h"
#include "Engine/Error.h"
#include "Engine/Coordinate.h"
#include "Engine/Math.h"
#include <string>

namespace Game
{

//                                                      Name          Element           Color         Melt    Boil    Density Conduct.    Opacity
const VoxelMap::Material VoxelMap::Material::vacuum   { L"Vacuum",    Element::None,    0xFF000000,   0,      0,      0,      0,          0.0        };
const VoxelMap::Material VoxelMap::Material::air      { L"Air",       Element::Air,     0xFFA08040,   60,     80,     1.225,  29.2/29.0,  0.01       };    // mix of N2 and O2, 29 g/mol
const VoxelMap::Material VoxelMap::Material::soil     { L"Soil",      Element::Nature,  0xFF20FF20,   0,      0,      1600,   0.4/42.0,   10         };     // 25% water 25% air, 50% solid = 42 g/mol
const VoxelMap::Material VoxelMap::Material::stone    { L"Stone",     Element::Stone,   0xFFA0A0A0,   1986,   3220,   2648,   10/60,      10         };     // for now 100% silicon dioxide, 60 g/mol
const VoxelMap::Material VoxelMap::Material::water    { L"Water",     Element::Water,   0xFFFF6010,   273,    373,    1000,   0.6065/18,  6.9/1000   };     // H2O 18 g/mol
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
    return Translucency() > 0.99;
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
    longitude(0),
    latitude(0),
    altitude(0),
    gravity(-10.0),
    airTemperature(0),
    waterLevel(0)
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
    this->longitude = x;
    this->latitude = y;
    this->altitude = z;
    voxels.resize(longitude * latitude * altitude);
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

void VoxelMap::Water(int level, double temperature)
{
    waterLevel = level;
    for (auto& voxel : voxels)
    {
        auto z = GetPosition(&voxel).z;
        if (z <= waterLevel)
        {
            if (voxel.material == &Material::vacuum)
            {
                voxel.material = &Material::water;
                voxel.mass = voxel.material->normalDensity * LiterPerBlock;
                voxel.temperature = temperature;
            }
        }
    }
}

void VoxelMap::Hill(float x, float y, float height, float stddev)
{
    Engine::Coordinate center(x, y, 0);

    for (unsigned xi = 0; xi < longitude; ++xi)
    {
        for (unsigned yi = 0; yi < latitude; ++yi)
        {
            int maxZ = int(std::round( height * Engine::Gaussian((Engine::Coordinate(float(xi), float(yi), 0) - center).Length(), stddev)));
            maxZ = std::min(maxZ, int(Altitude()));
            for (int zi = 0; zi < maxZ; ++zi)
            {
                auto& v = Get(Position(xi, yi, zi));
                v.material = &Material::stone;
                v.fixed = true;
                v.temperature = airTemperature; // Could lower based on distance to surface
                v.mass = v.material->normalDensity * LiterPerBlock;
            }
        }
    }
}

unsigned VoxelMap::Latitude() const
{
    return latitude;
}
unsigned VoxelMap::Longitude() const
{
    return longitude;
}
unsigned VoxelMap::Altitude() const
{
    return altitude;
}

unsigned VoxelMap::VoxelIndex(const Position& p) const
{
    const auto& stride = Stride();
    return p.x * stride.x + p.y * stride.y + p.z * stride.z;
}

const VoxelMap::Voxel& VoxelMap::Get(const Position& p) const
{   
    // TODO: get default air, water or rock, lava depending on which bound it's out and parameters
    return voxels.at(VoxelIndex(p));
}

VoxelMap::Voxel& VoxelMap::Get(const Position& p)
{   // can't get outside
    return voxels.at(VoxelIndex(p));
}

Position VoxelMap::Stride() const
{   // ordered Z,Y,X, in direction of gravity for best caching
    return Position(altitude*latitude, altitude, 1);
}

Position VoxelMap::GetPosition(const Voxel* pVoxel) const
{
    auto index = pVoxel - voxels.data();
    if ((index>=0) && (unsigned(index) < voxels.size()))
    {
        const auto& stride = Stride();
        int pz = (int(index) / stride.z) % altitude;
        int py = ((int(index) - (pz*stride.z)) / stride.y) % latitude;
        int px = ((int(index) - (pz*stride.z) - (py*stride.y)) / stride.x) % longitude;
        return Position(px, py, pz);
    }
    return Position();
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
    for (int i = std::min(p.z, int(Altitude())-1); i >= 0; --i)
    {
        const auto& v = Get(Position(p.x, p.y, i));
        if (!v.Gas())
        {
            Square s = v.Square();
            s.height = i;
            return s;
        }
    }
    return Square(Element::Fire, false, 0); // welcome to hell
}


void VoxelMap::Voxel::Render(const Position& p) const
{
    auto c = Color();
    c.Render();
    // south
    glPushName(LocationName(p, Direction::south));
    glNormal3d(Direction::south.Vector().x, Direction::south.Vector().z, Direction::south.Vector().y);
    glBegin(GL_QUADS);
        glVertex3f(0, 0, 0);
        glVertex3f(0, 1, 0);
        glVertex3f(1, 1, 0);
        glVertex3f(1, 0, 0);
    glEnd();
    glPopName();

    // north
    glPushName(LocationName(p, Direction::north));
    glNormal3d(Direction::north.Vector().x, Direction::north.Vector().z, Direction::north.Vector().y);
    glBegin(GL_QUADS);
        glVertex3f(0, 0, 1);
        glVertex3f(1, 0, 1);
        glVertex3f(1, 1, 1);
        glVertex3f(0, 1, 1);
    glEnd();
    glPopName();

    // east
    glPushName(LocationName(p, Direction::east));
    glNormal3d(Direction::east.Vector().x, Direction::east.Vector().z, Direction::east.Vector().y);
    glBegin(GL_QUADS);
        glVertex3f(1, 0, 0);
        glVertex3f(1, 1, 0);
        glVertex3f(1, 1, 1);
        glVertex3f(1, 0, 1);
    glEnd();
    glPopName();

    // west
    glPushName(LocationName(p, Direction::west));
    glNormal3d(Direction::west.Vector().x, Direction::west.Vector().z, Direction::west.Vector().y);
    glBegin(GL_QUADS);
        glVertex3f(0, 0, 0);
        glVertex3f(0, 0, 1);
        glVertex3f(0, 1, 1);
        glVertex3f(0, 1, 0);
    glEnd();
    glPopName();

    // top
    glPushName(LocationName(p, Direction::up));
    glNormal3d(Direction::up.Vector().x, Direction::up.Vector().z, Direction::up.Vector().y);
    glBegin(GL_QUADS);
        glVertex3f(0, 1, 0);
        glVertex3f(0, 1, 1);
        glVertex3f(1, 1, 1);
        glVertex3f(1, 1, 0);
    glEnd();
    glPopName();
}

void VoxelMap::Iterate(Position& p) const
{
    if (++p.z == Altitude())
    {
        p.z = 0;
        if (++p.y == Latitude())
        {
            p.y = 0;
            if (++p.x == Longitude())
            {
                p.x = 0;
            }
        }
    }
}

void VoxelMap::Render() const
{
    Position p;

    assert(glIsEnabled(GL_DEPTH_TEST));
    
    // Draw opaque
    glDisable(GL_BLEND);
    for (auto& v : voxels)
    {
        glPushMatrix();
        glScaled(MeterPerEl*HorizontalEl, MeterPerEl*VerticalEl, MeterPerEl*HorizontalEl);
        glTranslatef(float(p.x), float(p.z), float(p.y));
        if (v.Opaque())
        {
            v.Render(p);
        }
        // TODO: separate loop?
        if ((p.z == 0) && (!v.Opaque()))
        {   // lava bottom of the level
            glPushName(LocationName(p, Direction::up));
            glColor4ub(255, 50, 50, 255);
            glBegin(GL_QUADS);
                glVertex3f(0, 0, 0);
                glVertex3f(1, 0, 0);
                glVertex3f(1, 0, 1);
                glVertex3f(0, 0, 1);
            glEnd();
            glPopName();
        }
        Iterate(p);
        //glPopName();
        glPopMatrix();
    }
    glEnable(GL_BLEND);   
    assert(!p); // should be reset to 0,0,0

    for (auto& v : voxels)
    {
        glPushMatrix();
        glScaled(MeterPerEl*HorizontalEl, MeterPerEl*VerticalEl, MeterPerEl*HorizontalEl);
        glTranslatef(float(p.x), float(p.z), float(p.y));
        if (!v.Opaque() && !v.Transparent())
        {
            v.Render(p);
        }
        Iterate(p);
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
    int waterLevel;
    s >> temperature >> waterLevel;
    map.Water(waterLevel, temperature);
    map.Air(temperature);
    unsigned procedures;
    s >> procedures;
    for (unsigned p = 0; p < procedures; ++p)
    {
        std::wstring procedure;
        s >> procedure;
        if (procedure == L"HILL")
        {
            float x, y, height, stddev;
            s >> x >> y >> height >> stddev;
            map.Hill(x, y, height, stddev);
        }
        else
        {
            throw std::runtime_error("Unknown procedure");
        }
    }
    return s;
}

}
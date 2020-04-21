#include "stdafx.h"
#include "VoxelMap.h"
#include <gl/GL.h>
#include "Engine/Image.h"
#include "Engine/Error.h"
#include "Engine/Coordinate.h"
#include "Engine/Geometry.h"
#include "Engine/Line.h"
#include "Engine/Maths.h"
#include <string>

namespace Game
{

//                                                      Name          Element           Color         Melt    Boil    Density Conduct.    Granularity,  Opacity
const VoxelMap::Material VoxelMap::Material::vacuum   { L"Vacuum",    Element::None,    0xFF000000,   0,      0,      0,      0,          0,            0.0        };
const VoxelMap::Material VoxelMap::Material::air      { L"Air",       Element::Air,     0xFFA08040,   60,     80,     1.225,  29.2/29.0,  1,            0.01       };     // mix of N2 and O2, 29 g/mol
const VoxelMap::Material VoxelMap::Material::soil     { L"Soil",      Element::Nature,  0xFF20FF20,   0,      0,      1600,   0.4/65.0,   100,          10         };     // 65g/mol, based on 0% humidity. Part SiO2, N2 and proteins
const VoxelMap::Material VoxelMap::Material::stone    { L"Stone",     Element::Stone,   0xFFA0A0A0,   1986,   3220,   2648,   10/60,      1,            10         };     // for now 100% silicon dioxide, 60 g/mol
const VoxelMap::Material VoxelMap::Material::water    { L"Water",     Element::Water,   0xFFFF6010,   273,    373,    1000,   0.6065/18,  1,            6.9/1000   };     // H2O 18 g/mol
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
    if (temperature >= 500.0f)
    {
        return Game::Square(Element::Fire, Solid());
    }
    else
    {
        return Game::Square(material->element, Solid());
    }
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
    waterLevel(0),
    planetRadius(6.371e6)  // assume earth sized planet
{
    World(planetRadius);
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

void VoxelMap::World(double radius)
{
    planetRadius = radius;
    lava.material = &Material::stone;
    lava.temperature = float((lava.material->melt + lava.material->boil) / 2.0);
    lava.fixed = false;
    lava.mass = float(Engine::SphereVolume(planetRadius) * 1000.0 * lava.material->normalDensity);    // TODO: density of lava is not the same as stone
    constexpr double G = 6.67430e-11;
    gravity = G * lava.mass / (planetRadius*planetRadius);
}


VoxelMap::Directions::Directions() :
    flags(0)
{
}

VoxelMap::Directions& VoxelMap::Directions::operator|=(const Direction& dir)
{
    flags |= 1<< dir.Id();
    return *this;
}

bool VoxelMap::Directions::operator[](const Direction& dir) const
{
    return (flags & (1 << dir.Id()))!=0;
}

bool VoxelMap::Directions::empty() const
{
    return flags == 0;
}

void VoxelMap::Air(double temperature, double meters)
{
    atmosphere.temperature = float(temperature);
    atmosphere.material = &Material::air;
    atmosphere.fixed = false;
    atmosphere.mass = float((Engine::SphereVolume(meters + planetRadius) - Engine::SphereVolume(planetRadius))*
        1000.0 * atmosphere.material->normalDensity * 0.5);    // TODO: density isn't even over the whole atmosphere. assumed average

    for (auto& voxel : voxels)
    {
        if (voxel.material == &Material::vacuum)
        {
            voxel.material = &Material::air;
            voxel.mass = float(voxel.material->normalDensity * LiterPerBlock);
            voxel.temperature = atmosphere.temperature;
        }
    }
}

void VoxelMap::Water(int level, double temperature)
{
    water.temperature = float(temperature);
    water.material = &Material::water;
    water.fixed = false;
    water.mass = 50e15f; // roughly the north  sea

    waterLevel = level;
    for (auto& voxel : voxels)
    {
        auto z = GetPosition(&voxel).z;
        if (z <= waterLevel)
        {
            if (voxel.material == &Material::vacuum)
            {
                voxel.material = &Material::water;
                voxel.mass = float(Material::water.normalDensity * LiterPerBlock); // TODO: temperature
                voxel.temperature = float(temperature);
                voxel.humidity = voxel.mass;
            }
            else if (voxel.Gas())
            {
                voxel.material = &Material::water;
                voxel.mass = float(Material::water.normalDensity * LiterPerBlock); // TODO: temperature
                voxel.temperature = float(temperature);
                voxel.humidity = voxel.mass;    // TODO: move gas
            }
        }
    }
}

void VoxelMap::Hill(const Engine::Coordinate& p1, const Engine::Coordinate& p2, float stddev)
{
    Engine::Line ridge_line(Engine::Coordinate(p1.x, p1.y, 0),
                    Engine::Coordinate(p2.x, p2.y, 0));

    for (unsigned xi = 0; xi < longitude; ++xi)
    {
        for (unsigned yi = 0; yi < latitude; ++yi)
        {
            Engine::Coordinate c(float(xi), float(yi), 0);
            double distance = Engine::Distance(c, ridge_line);
            double interpolation_factor = std::max(0.0,std::min(1.0, ridge_line.Vector().Dot(c - ridge_line.a) / ridge_line.Vector().LengthSquared()));
            double height = Engine::Lerp(double(p1.z), double(p2.z), interpolation_factor) * Engine::Gaussian(distance, stddev);
            int maxZ = int(std::round( height ));
            maxZ = std::min(maxZ, int(Altitude()));
            for (int zi = 0; zi < maxZ; ++zi)
            {
                auto& v = Get(Position(xi, yi, zi));
                v.material = &Material::stone;
                v.fixed = true;
                v.temperature = atmosphere.temperature; // Could lower based on distance to surface
                v.mass = float(v.material->normalDensity * LiterPerBlock);
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
    if (p.z < 0)
    {
        if (Get(Position(p.x, p.y, 0)).material == &Material::water)
            return water;
        else
            return lava;
    }
    if (unsigned(p.z) >= Altitude())
        return atmosphere;
    if (p.x < 0 || p.y < 0 || unsigned(p.x) >= Longitude() || unsigned(p.y) >= Latitude())
    {
        if (p.z < waterLevel)
            return water;
        else
            return atmosphere;  // TODO: rock level needs a profile on each edge
    }
    // TODO: get default air, water or rock, lava depending on which bound it's out and parameters
    return voxels.at(VoxelIndex(p));
}

VoxelMap::Voxel& VoxelMap::Get(const Position& p)
{   // can't get outside
    return voxels.at(VoxelIndex(p));
}

VoxelMap::Directions VoxelMap::Visibility(const Position& p) const
{
    Directions result;
    for (const Direction& direction : Direction::all)
    {
        if (!Get(p + direction.Vector()).Opaque())
        {
            result|=direction;
        }
    }
    return result;
}

void VoxelMap::Compute()
{
    Position p;
    for (auto& voxel : voxels)
    {
        voxel.visibility = Visibility(p);
        Iterate(p);
    }

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
    return lava.Square(); // welcome to hell
}


void VoxelMap::Voxel::Render(const Position& p, const Directions& visibility) const
{
    auto c = Color();
    c.Render();
    // south
    if (visibility[Direction::south])
    {
        glPushName(LocationName(p, Direction::south));
        glNormal3d(0, 0, -1); //  Direction::south.Vector()
        glBegin(GL_QUADS);
            glVertex3f(0, 0, 0);
            glVertex3f(0, 1, 0);
            glVertex3f(1, 1, 0);
            glVertex3f(1, 0, 0);
        glEnd();
        glPopName();
    }
    // north
    if (visibility[Direction::north])
    {
        glPushName(LocationName(p, Direction::north));
        glNormal3d(0, 0, 1);
        glBegin(GL_QUADS);
            glVertex3f(0, 0, 1);
            glVertex3f(1, 0, 1);
            glVertex3f(1, 1, 1);
            glVertex3f(0, 1, 1);
        glEnd();
        glPopName();
    }
    // east
    if (visibility[Direction::east])
    {
        glPushName(LocationName(p, Direction::east));
        glNormal3d(1, 0, 0);
        glBegin(GL_QUADS);
            glVertex3f(1, 0, 0);
            glVertex3f(1, 1, 0);
            glVertex3f(1, 1, 1);
            glVertex3f(1, 0, 1);
        glEnd();
        glPopName();
    }
    // west
    if (visibility[Direction::west])
    {
        glPushName(LocationName(p, Direction::west));
        glNormal3d(-1, 0, 0);
        glBegin(GL_QUADS);
            glVertex3f(0, 0, 0);
            glVertex3f(0, 0, 1);
            glVertex3f(0, 1, 1);
            glVertex3f(0, 1, 0);
        glEnd();
        glPopName();
    }
    // top
    if (visibility[Direction::up])
    {
        glPushName(LocationName(p, Direction::up));
        glNormal3d(0, 1, 0);
        glBegin(GL_QUADS);
            glVertex3f(0, 1, 0);
            glVertex3f(0, 1, 1);
            glVertex3f(1, 1, 1);
            glVertex3f(1, 1, 0);
        glEnd();
        glPopName();
    }
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
    glEnable(GL_LIGHTING);
    for (auto& v : voxels)
    {
        glPushMatrix();
        glScaled(MeterPerEl*HorizontalEl, MeterPerEl*VerticalEl, MeterPerEl*HorizontalEl);
        glTranslatef(float(p.x), float(p.z), float(p.y));
        if (v.Opaque())
        {
            auto visibility = v.visibility;
            if (!visibility.empty())
            {
                v.Render(p, visibility);
            }
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
    assert(!p); // should be reset to 0,0,0

    // translucent pass
    glEnable(GL_BLEND);
    glDisable(GL_LIGHTING);
    for (auto& v : voxels)
    {
        glPushMatrix();
        glScaled(MeterPerEl*HorizontalEl, MeterPerEl*VerticalEl, MeterPerEl*HorizontalEl);
        glTranslatef(float(p.x), float(p.z), float(p.y));
        if (!v.Opaque() && !v.Transparent())
        {
            auto visibility = v.visibility;
            if (!visibility.empty())
            {
                v.Render(p, visibility);
            }
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
    map.Air(temperature, 20000);
    unsigned procedures;
    s >> procedures;
    for (unsigned p = 0; p < procedures; ++p)
    {
        std::wstring procedure;
        s >> procedure;
        if (procedure == L"HILL")
        {
            Engine::Coordinate p0, p1;
            s >> p0 >> p1;
            float stddev;
            s >> stddev;
            map.Hill(p0, p1, stddev);
        }
        else
        {
            throw std::runtime_error("Unknown procedure");
        }
    }
    map.Compute();
    return s;
}

}
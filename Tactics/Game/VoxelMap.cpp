#include "stdafx.h"
#include "VoxelMap.h"
#include <gl/GL.h>
#include "Engine/Image.h"
#include "Engine/Error.h"
#include "Engine/Coordinate.h"
#include "Engine/Geometry.h"
#include "Engine/Line.h"
#include "Engine/Maths.h"
#include "Engine/Random.h"
#include "Engine/Drawing.h"
#include "Engine/Text.h"
#include <string>

namespace Game
{

//                                                      Name          Element           Color         Melt    Boil    Density MolasMass     Conduct.    Surface,    Hardness,   Granularity,  Opacity
const VoxelMap::Material VoxelMap::Material::vacuum   { L"Vacuum",    Element::None,    0xFF000000,   0,      0,      0,      0,            0,          0,           0,         0,            0.0        };
const VoxelMap::Material VoxelMap::Material::air      { L"Air",       Element::Air,     0xFFA08040,   60,     80,     1.225,  29,           29.2,       10e-3,       1e9,       1,            0.01       };     // mix of N2 and O2, 29 g/mol
const VoxelMap::Material VoxelMap::Material::soil     { L"Soil",      Element::Nature,  0xFF20FF20,   0,      0,      1600,   65,           0.4,        0.7,         1e7,       100,          10         };     // 65g/mol, based on 0% humidity. Part SiO2, N2 and proteins
const VoxelMap::Material VoxelMap::Material::stone    { L"Stone",     Element::Stone,   0xFFA0A0A0,   1986,   3220,   2648,   60,           10,         0.8,         1e10,      1,            10         };     // for now 100% silicon dioxide, 60 g/mol
const VoxelMap::Material VoxelMap::Material::water    { L"Water",     Element::Water,   0xFFFF6010,   273,    373,    1000,   18,           0.6065,     0.07,        9e9,       1,            6.9/1000   };     // H2O 18 g/mol
// redundant for now VoxelMap::Material VoxelMap::Material::sand  { L"Sand",      1986,   3220,   2648,        10,            0.0           };     // silicon dioxide, 60 g/mol

constexpr double PascalPerAtmosphere = 101325.0;

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
    auto color = material->color.Translucent(1.0-Translucency());

    return color;
}

VoxelMap::VoxelMap() :
    longitude(0),
    latitude(0),
    altitude(0),
    gravity(-10.0),
    planetRadius(6.371e6)  // assume earth sized planet
{
    World(planetRadius);
}

VoxelMap::Voxel::Voxel() :
    material(&VoxelMap::Material::vacuum),
    temperature(0),
    mass(0),
    flow(0, 0, 0),
    fixed(false),
    boundary(false)
{
}

void VoxelMap::Space(unsigned x, unsigned y, unsigned z)
{
    this->longitude = x+2;
    this->latitude = y+2;
    this->altitude = z+2;
    voxels.resize(longitude * latitude * altitude);
    for (unsigned x = 0; x < longitude; ++x)
    {
        for (unsigned y = 0; y < latitude; ++y)
        {
            Get(Position(x, y, 0)).boundary = true;
            Get(Position(x, y, altitude - 1)).boundary = true;
        }
        for (unsigned z = 0; z < altitude; ++z)
        {
            Get(Position(x, 0, z)).boundary = true;
            Get(Position(x, latitude - 1, z)).boundary = true;
        }
    }
}

void VoxelMap::World(double radius)
{
    planetRadius = radius;
    for (unsigned x = 0; x < longitude; ++x)
    {
        for (unsigned y = 0; y < latitude; ++y)
        {
            auto& lava = Get(Position(x, y, 0));
            lava.material = &Material::stone;
            lava.temperature = float((lava.material->melt + lava.material->boil) / 2.0);
            lava.fixed = true;
            lava.mass = float(lava.material->normalDensity * LiterPerBlock);
        }
    }
    constexpr double coreDensity = 5000; // g/L approximate density of earth. liquid iron is 7000 
    constexpr double G = 6.67430e-11 * 0.001;  // m^3 / g * s^2
    double worldMass = float(Engine::SphereVolume(planetRadius) * 1000.0 * coreDensity);
    // surface gravity: https://en.wikipedia.org/wiki/Surface_gravity
    gravity = G * worldMass / (planetRadius*planetRadius);
}


VoxelMap::Directions::Directions() :
    Directions(0)
{
}

VoxelMap::Directions::Directions(uint16_t flags) :
    flags(flags)
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
//    atmosphere.mass = float((Engine::SphereVolume(meters + planetRadius) - Engine::SphereVolume(planetRadius))*
//        1000.0 * atmosphere.material->normalDensity * 0.5);    // TODO: density isn't even over the whole atmosphere. assumed average

    for (auto& voxel : voxels)
    {
        if (voxel.material == &Material::vacuum)
        {
            voxel.material = &Material::air;
            voxel.mass = float(voxel.material->normalDensity * LiterPerBlock);
            voxel.temperature = float(temperature);
            voxel.fixed = false;
        }
    }
}

void VoxelMap::Water(int level, double temperature)
{
    for (auto& voxel : voxels)
    {
        auto z = GetPosition(&voxel).z;
        if (z < level)
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

void VoxelMap::Hill(Engine::Coordinate p1, Engine::Coordinate p2, float stddev)
{
    p1 += Engine::Vector(1, 1, 0);  // offset, but z is offset in maxZ for better rounding
    p2 += Engine::Vector(1, 1, 0);
    Engine::Line ridge_line(Engine::Coordinate(p1.x, p1.y, 0),
                    Engine::Coordinate(p2.x, p2.y, 0));
    // offset the hill for external coordinates excluding boundaries
    for (unsigned xi = 0; xi < longitude; ++xi)
    {
        for (unsigned yi = 0; yi < latitude; ++yi)
        {
            Engine::Coordinate c(float(xi), float(yi), 0);
            double distance = Engine::Distance(c, ridge_line);
            double interpolation_factor = std::max(0.0,std::min(1.0, ridge_line.Vector().Dot(c - ridge_line.a) / ridge_line.Vector().LengthSquared()));
            double height = Engine::Lerp(double(p1.z), double(p2.z), interpolation_factor) * Engine::Gaussian(distance, stddev);
            int maxZ = 1+int(std::round( height ));
            maxZ = std::min(maxZ, int(Altitude()));
            for (int zi = 0; zi < maxZ; ++zi)
            {
                auto& v = Get(Position(xi, yi, zi));
                v.material = &Material::stone;
                v.fixed = true;
                v.mass = float(v.material->normalDensity * LiterPerBlock);
                assert(v.temperature > 0);  // hill in a vacuum is cold 
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
        auto neighbourPosition = p + direction.Vector();

        if (!IsInside(neighbourPosition) || 
            !Get(neighbourPosition).Opaque())
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
{   // ordered Z,Y,X, in direction of iy for best caching
    return Position(altitude*latitude, altitude, 1);
}

bool VoxelMap::IsBoundary(const Position& p) const
{
    return p.x == 0 ||
        p.y == 0 ||
        p.z == 0 ||
        p.x == longitude - 1 ||
        p.y == latitude - 1 ||
        p.z == altitude - 1;

}

bool VoxelMap::IsInside(const Position& p) const
{
    return p.x >= 0 &&
        p.y >= 0 &&
        p.z >= 0 &&
        unsigned(p.x) < longitude &&
        unsigned(p.y) < latitude &&
        unsigned(p.z) < altitude;
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
    if (p.y < 0)
        return Square();
    Position adjusted =  p + Position(1, 1, 1);
    if (unsigned(adjusted.x) >= Longitude())
        return Square();
    if (unsigned(adjusted.y) >= Latitude())
        return Square();
    for (int i = std::min(adjusted.z, int(Altitude()-1)); i > 0; --i)
    {
        const auto& v = Get(Position(adjusted.x, adjusted.y, i));
        if (v.Solid())
        {
            Square s = v.Square(i);
            return s;
        }
    }
    return Get(Position(adjusted.x,adjusted.y,0)).Square(0); // welcome to hell
}


void VoxelMap::Voxel::Render(const Position& p, const Directions& visibility, bool analysis) const
{
    auto c = Color();
    c.Render();
    unsigned mode = analysis? GL_LINE_LOOP:GL_QUADS; 
    if (visibility[Direction::south])
    {
        glPushName(LocationName(p, Direction::south));
        glNormal3d(0, 0, -1); //  Direction::south.Vector()
        glBegin(mode);
            glVertex3d(0, 0* MeterPerEl, 0);
            glVertex3d(0, 1* MeterPerEl, 0);
            glVertex3d(1, 1* MeterPerEl, 0);
            glVertex3d(1, 0* MeterPerEl, 0);
        glEnd();
        glPopName();
    }
    // north
    if (visibility[Direction::north])
    {
        glPushName(LocationName(p, Direction::north));
        glNormal3d(0, 0, 1);
        glBegin(mode);
            glVertex3d(0, 0* MeterPerEl, 1);
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
            Engine::glText(std::to_string(Pressure()/PascalPerAtmosphere));
            glDrawArrow(flow);
        glPopMatrix();
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

double VoxelMap::DiffusionRate(const Position& p, const Direction& d) const
{   //  TODO: physics https://en.wikipedia.org/wiki/Mass_diffusivity;
    //  especiall when each voxel can contain multiple materials
    auto neightbourPosition = p + d.Vector();
    if (!IsInside(neightbourPosition))
    {
        return 0;   // perform diffuction to regain lost mass here too? or just source it
    }
    const auto & v = Get(p);
    if (v.Solid())
        return 0;
    const auto & n = Get(neightbourPosition);
    if (n.Solid())
        return 0;
    if (v.Gas() && n.Gas())
        return 0.1;
    else if (v.material == n.material)
        return 0.01;
    else
        return 0;
}

void VoxelMap::ComputeFlow(double seconds)
{
    // Flow
    Position p;
    for (auto& v : voxels)
    {
        if (!IsBoundary(p))
        {
            auto pressure = v.Pressure();
            for (auto d : Direction::all)
            {
                auto neightbourPosition = p + d.Vector();
                const auto& n = Get(neightbourPosition);
                // presssure (Pa) is kg/
                double difference = d.Surface() * (pressure - n.Pressure());

                // Pressure difference (Pa) is Kg/(m/s^2) 
                //  multiplied with time it's the change in velocity
                v.flow += Engine::Vector(
                    d.Vector().x,
                    d.Vector().y,
                    d.Vector().z
                ) * (difference * seconds / v.mass);
                if (d == Direction::down)
                {
                    if (!Get(neightbourPosition).fixed)
                    {
                        v.flow += Engine::Vector(0, 0, -gravity)*seconds;
                    }
                }
            }
        }
    }
}

void VoxelMap::Flow(double seconds)
{   // https://en.wikipedia.org/wiki/Mass_flux
    Position p;
    for (auto& v : voxels)
    {
        if (!IsBoundary(p))
        {
            for (auto d : Direction::all)
            {
                Engine::Vector neighbourDirection(
                    d.Vector().x,
                    d.Vector().y,
                    d.Vector().z*MeterPerEl
                );
                auto velocity = v.flow.Dot(neighbourDirection);
                if (velocity > 0)
                {
                    float mass = float(velocity * seconds);
                    auto neighbourPosition = p + d.Vector();
                    if (neighbourPosition.z >= 0)    // TODO: more subtle bottom of the world
                    {
                        v.mass = std::max(0.0f, v.mass - mass);
                        auto& neighbour = Get(p + d.Vector());
                        if (!neighbour.boundary)
                        {
                            neighbour.mass += mass;
                        }
                    }
                }
            }
        }
        Iterate(p);
    }
}

void VoxelMap::Diffuse(double seconds)
{
    Position p;
    for (auto& v : voxels)
    {
        if (IsBoundary(p))
        {   // crudely diffuse 100% into boundaries
            // Can add wind and water flows here with 
            v.mass = float(v.material->normalDensity * LiterPerBlock);
        }
        else
        {
            for (auto d : Direction::positive)
            {   // propagate in positive direction but symetrical saves a copy of the whole map and assures conservation of mass
                double rate = DiffusionRate(p, d);
                assert(rate == DiffusionRate(p, d.Opposite())); // conservation of mass 
                if (rate != 0)
                {
                    assert(rate > 0);   // don't know what negative diffusion means
                    auto& neighbour = Get(p + d.Vector());
                    auto delta = (v.mass - neighbour.mass)*d.Surface()*rate*seconds;
                    neighbour.mass += delta;
                    v.mass -= delta;
                }
            }
        }
        Iterate(p);
    }

}

void VoxelMap::Tick(double seconds)
{
    // Fluid dynamics:
    //  https://en.wikipedia.org/wiki/Fluid_dynamics
    //  With incompressible flow, as long as it stays under 111m/s (mach 0.3) is simpler
    Diffuse(seconds);
}

void VoxelMap::Render() const
{
    //RenderPretty();
    RenderAnalysis();
}


void VoxelMap::RenderPretty() const
{
    assert(glIsEnabled(GL_DEPTH_TEST));

    Position p;
    // Draw opaque
    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
    for (auto& v : voxels)
    {
        glPushMatrix();
        glTranslated(p.x-1, (p.z-1)*MeterPerEl, p.y-1); // offset by -1,-1,-1 for boundary
        if (v.Opaque())
        {
            auto visibility = v.visibility;
            if (!visibility.empty())
            {
                v.Render(p, visibility, false);
            }
        }
        // TODO: separate loop?
        if ((p.z == 0) && (!v.Opaque()))
        {   // lava bottom of the level
            glPushName(LocationName(p, Direction::up));
            glColor4ub(255, 50, 50, 255);
            glBegin(GL_QUADS);
                glVertex3f(0, 0 * MeterPerEl, 0);
                glVertex3f(1, 0 * MeterPerEl, 0);
                glVertex3f(1, 0 * MeterPerEl, 1);
                glVertex3f(0, 0 * MeterPerEl, 1);
            glEnd();
            glPopName();
        }
        Iterate(p);
        glPopMatrix();
    }
    assert(!p); // should be reset to 0,0,0

    // translucent pass
    glEnable(GL_BLEND);
    glDisable(GL_LIGHTING);
    for (auto& v : voxels)
    {
        glPushMatrix();
        glTranslatef(float(p.x), float(p.z), float(p.y));
        if (!v.Opaque() && !v.Transparent())
        {
            auto visibility = v.visibility;
            if (!visibility.empty())
            {
                v.Render(p, visibility, false);
            }
        }
        Iterate(p);
        glPopMatrix();
    }
    Engine::CheckGLError();
}


void VoxelMap::RenderAnalysis() const
{
    assert(glIsEnabled(GL_DEPTH_TEST));

    Position p;
    // Draw opaque
    glEnable(GL_BLEND);
    glDisable(GL_LIGHTING);
    for (auto& v : voxels)
    {
        glPushMatrix();
        glTranslated(p.x, p.z*MeterPerEl, p.y);
        v.Render(p, Directions(0xFFFF), true);
        Iterate(p);
        glPopMatrix();
    }
    assert(!p); // should be reset to 0,0,0
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
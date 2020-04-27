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

VoxelMap::VoxelMap() :
    voxels(),
    gravity(-10.0),
    planetRadius(6.371e6)  // assume earth sized planet
{
    World(planetRadius);
}



void VoxelMap::Space(unsigned x, unsigned y, unsigned z)
{
    voxels = Data(x + 2, y + 2, z + 2);

    for (unsigned x = 0; x < voxels.Longitude(); ++x)
    {
        for (unsigned y = 0; y < voxels.Latitude(); ++y)
        {
            voxels[Position(x, y, 0)].boundary = true;
            voxels[Position(x, y, voxels.Altitude() - 1)].boundary = true;
        }
        for (unsigned z = 0; z < voxels.Altitude(); ++z)
        {
            voxels[Position(x, 0, z)].boundary = true;
            voxels[Position(x, voxels.Latitude()- 1, z)].boundary = true;
        }
    }
}

void VoxelMap::World(double radius)
{
    planetRadius = radius;
    for (unsigned x = 0; x < voxels.Longitude(); ++x)
    {
        for (unsigned y = 0; y < voxels.Latitude(); ++y)
        {
            auto& lava = voxels[Position(x, y, 0)];
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
        if (voxel->material == &Material::vacuum)
        {
            voxel->material = &Material::air;
            voxel->mass = float(voxel->material->normalDensity * LiterPerBlock);
            voxel->temperature = float(temperature);
            voxel->fixed = false;
        }
    }
}

void VoxelMap::Water(int level, double temperature)
{
    for (auto& voxel : voxels)
    {
        auto z = voxel.position.z;
        if (z < level)
        {
            if (voxel->material == &Material::vacuum)
            {
                voxel->material = &Material::water;
                voxel->mass = float(Material::water.normalDensity * LiterPerBlock); // TODO: temperature
                voxel->temperature = float(temperature);
                voxel->humidity = voxel->mass;
            }
            else if (voxel->Gas())
            {
                voxel->material = &Material::water;
                voxel->mass = float(Material::water.normalDensity * LiterPerBlock); // TODO: temperature
                voxel->temperature = float(temperature);
                voxel->humidity = voxel->mass;    // TODO: move gas
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
    for (unsigned xi = 0; xi < voxels.Longitude(); ++xi)
    {
        for (unsigned yi = 0; yi < voxels.Latitude(); ++yi)
        {
            Engine::Coordinate c(float(xi), float(yi), 0);
            double distance = Engine::Distance(c, ridge_line);
            double interpolation_factor = std::max(0.0,std::min(1.0, ridge_line.Vector().Dot(c - ridge_line.a) / ridge_line.Vector().LengthSquared()));
            double height = Engine::Lerp(double(p1.z), double(p2.z), interpolation_factor) * Engine::Gaussian(distance, stddev);
            int maxZ = 1+int(std::round( height ));
            maxZ = std::min(maxZ, int(voxels.Altitude()));
            for (int zi = 0; zi < maxZ; ++zi)
            {
                auto& v = voxels[Position(xi, yi, zi)];
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
    return voxels.Latitude();
}
unsigned VoxelMap::Longitude() const
{
    return voxels.Longitude();
}


VoxelMap::Directions VoxelMap::Visibility(const Position& p) const
{
    Directions result;
    for (const Direction& direction : Direction::all)
    {
        auto neighbourPosition = p + direction.Vector();

        if (!voxels.IsInside(neighbourPosition) || 
            !voxels[neighbourPosition].Opaque())
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
        voxel->visibility = Visibility(voxel.position);
    }

}

bool VoxelMap::IsBoundary(const Position& p) const
{
    return p.x == 0 ||
        p.y == 0 ||
        p.z == 0 ||
        p.x == voxels.Longitude() - 1 ||
        p.y == voxels.Latitude() - 1 ||
        p.z == voxels.Altitude()- 1;

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
    for (int i = std::min(adjusted.z, int(voxels.Altitude()-1)); i > 0; --i)
    {
        const auto& v = voxels[Position(adjusted.x, adjusted.y, i)];
        if (v.Solid())
        {
            Square s = v.Square(i);
            return s;
        }
    }
    return voxels[Position(adjusted.x,adjusted.y,0)].Square(0); // welcome to hell
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


double VoxelMap::DiffusionRate(const Position& p, const Direction& d) const
{   //  TODO: physics https://en.wikipedia.org/wiki/Mass_diffusivity;
    //  especiall when each voxel can contain multiple materials
    auto neightbourPosition = p + d.Vector();
    if (!voxels.IsInside(neightbourPosition))
    {
        return 0;   // perform diffuction to regain lost mass here too? or just source it
    }
    const auto & v = voxels[p];
    if (v.Solid())
        return 0;
    const auto & n = voxels[neightbourPosition];
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
            auto pressure = v->Pressure();
            for (auto d : Direction::all)
            {
                auto neightbourPosition = p + d.Vector();
                const auto& n = voxels[neightbourPosition];
                // presssure (Pa) is kg/
                double difference = d.Surface() * (pressure - n.Pressure());

                // Pressure difference (Pa) is Kg/(m/s^2) 
                //  multiplied with time it's the change in velocity
                v->flow += Engine::Vector(
                    d.Vector().x,
                    d.Vector().y,
                    d.Vector().z
                ) * (difference * seconds / v->mass);
                if (d == Direction::down)
                {
                    if (!voxels[neightbourPosition].fixed)
                    {
                        v->flow += Engine::Vector(0, 0, -gravity)*seconds;
                    }
                }
            }
        }
    }
}

void VoxelMap::Flow(double seconds)
{   // https://en.wikipedia.org/wiki/Mass_flux
    for (auto& v : voxels)
    {
        if (!IsBoundary(v.position))
        {
            for (auto d : Direction::all)
            {
                Engine::Vector neighbourDirection(
                    d.Vector().x,
                    d.Vector().y,
                    d.Vector().z*MeterPerEl
                );
                auto velocity = v->flow.Dot(neighbourDirection);
                if (velocity > 0)
                {
                    float mass = float(velocity * seconds);
                    auto neighbourPosition = v.position + d.Vector();
                    if (neighbourPosition.z >= 0)    // TODO: more subtle bottom of the world
                    {
                        v->mass = std::max(0.0f, v->mass - mass);
                        auto& neighbour = voxels[neighbourPosition];
                        if (!neighbour.boundary)
                        {
                            neighbour.mass += mass;
                        }
                    }
                }
            }
        }
    }
}

void VoxelMap::Diffuse(double seconds)
{
    for (auto& v : voxels)
    {
        if (IsBoundary(v.position))
        {   // crudely diffuse 100% into boundaries
            // Can add wind and water flows here with 
            v->mass = float(v->material->normalDensity * LiterPerBlock);
        }
        else
        {
            for (auto d : Direction::positive)
            {   // propagate in positive direction but symetrical saves a copy of the whole map and assures conservation of mass
                double rate = DiffusionRate(v.position, d);
                assert(rate == DiffusionRate(v.position, d.Opposite())); // conservation of mass, but normally need to do both directions 
                if (rate != 0)
                {
                    assert(rate > 0);   // don't know what negative diffusion means
                    auto& neighbour = voxels[v.position + d.Vector()];
                    auto delta = float(v->mass - neighbour.mass)*d.Surface()*rate*seconds;
                    neighbour.mass += delta;
                    v->mass -= delta;
                }
            }
        }
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

    // Draw opaque
    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
    for (auto& v : voxels)
    {
        glPushMatrix();
        glTranslated(v.position.x-1, (v.position.z-1)*MeterPerEl, v.position.y-1); // offset by -1,-1,-1 for boundary
        if (v->Opaque())
        {
            auto visibility = v->visibility;
            if (!visibility.empty())
            {
                v->Render(v.position, visibility, false);
            }
        }
        glPopMatrix();
    }

    // translucent pass
    glEnable(GL_BLEND);
    glDisable(GL_LIGHTING);
    for (auto& v : voxels)
    {
        glPushMatrix();
        glTranslated(v.position.x - 1, (v.position.z - 1)*MeterPerEl, v.position.y - 1); // offset by -1,-1,-1 for boundary
        if (!v->Opaque() && !v->Transparent())
        {
            auto visibility = v->visibility;
            if (!visibility.empty())
            {
                v->Render(v.position, visibility, false);
            }
        }
        glPopMatrix();
    }
    Engine::CheckGLError();
}


void VoxelMap::RenderAnalysis() const
{
    assert(glIsEnabled(GL_DEPTH_TEST));

    // Draw opaque
    glEnable(GL_BLEND);
    glDisable(GL_LIGHTING);
    for (auto& v : voxels)
    {
        glPushMatrix();
        glTranslated(v.position.x-1, (v.position.z-1)*MeterPerEl, v.position.y-1);
        v->Render(v.position, Directions(0xFFFF), true);
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
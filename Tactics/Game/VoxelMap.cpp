#include "stdafx.h"
#include <numeric>
#include "VoxelMap.h"
#include <gl/GL.h>
#include "Engine/Image.h"
#include "Engine/Error.h"
#include "Engine/Coordinate.h"
#include "Engine/Geometry.h"
#include "Engine/Line.h"
#include "Engine/Timer.h"
#include "Engine/Maths.h"
#include "Engine/Random.h"
#include "Engine/Drawing.h"
#include "Engine/Text.h"
#include "Engine/Mesh.h"
#include "Engine/Matrix.h"
#include "Engine/Plane.h"
#include "Engine/Utils.h"
#include "Engine/AxisAlignedBoundingBox.h"
#include <string>

namespace Game
{

VoxelMap::VoxelMap() : 
    VoxelMap(0, 0, 0)
{
}

VoxelMap::VoxelMap(unsigned longitude, unsigned latitude, unsigned altitude) :
    voxels(longitude, latitude, altitude),
    time(0),
    gravity(-10.0),
    planetRadius(6.371e6),  // assume earth sized planet
    atmosphereRadius(2e5),  // assume earth atmosphere
    atmosphericTemperature(273.15f),
    wind(0, 0, 0)
{
    World(planetRadius);
}

VoxelMap::~VoxelMap() = default;

void VoxelMap::Space(unsigned x, unsigned y, unsigned z)
{
    voxels = Data(x, y, z);
}

void VoxelMap::World(double radius)
{
    planetRadius = radius;
    for (int x = 0; x < voxels.GetSize().x; ++x)
    {
        for (int y = 0; y < voxels.GetSize().y; ++y)
        {
            voxels.SetPressure(Position(x, y, -1),
                Material::stone,
                (Material::stone.melt + Material::stone.boil) / 2.0,
                PascalPerAtmosphere);
        }
    }
    constexpr double coreDensity = 5000; // g/L approximate density of earth. liquid iron is 7000 
    constexpr double G = 6.67430e-11 * 0.001;  // m^3 / g * s^2
    double worldMass = float(Engine::SphereVolume(planetRadius) * 1000.0 * coreDensity);
    // surface gravity: https://en.wikipedia.org/wiki/Surface_gravity
    gravity = G * worldMass / (planetRadius*planetRadius);
}

double VoxelMap::Elevation(int z) const
{
    return voxels.Center(Position(0,0,z)).z;
}

float VoxelMap::AtmosphericTemperature(double elevation) const
{
    double atmosphericLapse = atmosphericTemperature / atmosphereRadius;
    return float(atmosphericTemperature + atmosphericLapse * elevation);
}

float VoxelMap::AtmosphericPressure(double elevation) const
{
    return float(PascalPerAtmosphere * (1.0- elevation/ atmosphereRadius));
}

void VoxelMap::Air(double temperature, double meters)
{
//    atmosphere.mass = float((Engine::SphereVolume(meters + planetRadius) - Engine::SphereVolume(planetRadius))*
//        1000.0 * atmosphere.material->normalDensity * 0.5);    // TODO: density isn't even over the whole atmosphere. assumed average
    atmosphericTemperature = float(temperature);
    atmosphereRadius = meters;
    double atmorphericLapse = float(-temperature / atmosphereRadius);
    auto bounds = voxels.Bounds();
    auto gs = voxels.GridSize();
    size_t dbgCount = 0;
    for(int y=bounds.z.begin; y<bounds.z.end;++y)
    {
        Engine::Box layer(Engine::AABB(
            Engine::Range<double>(bounds.x.begin, bounds.x.end)*gs.x,
            Engine::Range<double>(bounds.y.begin, bounds.y.end)*gs.y,
            Engine::Range<double>(y, y+1)*gs.z));
        auto temperature = AtmosphericTemperature(Elevation(y));
        dbgCount += voxels.Fill(layer, Material::air, temperature);
    }
    OutputDebugStringW(std::wstring(L"Air =" + std::to_wstring(dbgCount) + L" voxels\n").c_str());
}

void VoxelMap::Wind(const Engine::Vector& speed)
{
    wind = speed;
    // TODO: set all air flux and boundary
}

void VoxelMap::Sea(int level, double temperature)
{
    auto bounds = voxels.Bounds();
    auto gs = voxels.GridSize();
    Engine::Box sea(Engine::AABB(
        Engine::Range<double>(bounds.x.begin, bounds.x.end)*gs.x, 
        Engine::Range<double>(bounds.y.begin, bounds.y.end)*gs.y, 
        Engine::Range<double>(bounds.z.begin, level)*gs.z));
    auto dbgCount = voxels.Fill(sea, Material::water, temperature);
    OutputDebugStringW((std::wstring(L"Sea level ") + Engine::ToWString(level) + L"=" + std::to_wstring(dbgCount) + L" voxels\n").c_str());
}

class Hill : public Engine::IVolume
{
public:
    Hill(const Engine::Line& line, double stddev) :
        m_line(line),
        m_stddev(stddev)
    {
    }
    Engine::AABB GetBoundingBox() const override
    {
        constexpr double treshold = 0.1;    // should be grid/z / 2 but don't know what grid
        double aHorRange=0, bHorRange = 0;
        if (m_line.a.z >= treshold)
            aHorRange = Engine::InvGaussian(treshold / m_line.a.z, m_stddev);
        if (m_line.b.z >= treshold)
            bHorRange = Engine::InvGaussian(treshold / m_line.b.z, m_stddev);
        Engine::AABB aBounds(Engine::Range<double>(m_line.a.x - aHorRange, m_line.a.x + aHorRange),
            Engine::Range<double>(m_line.a.y - aHorRange, m_line.a.y + aHorRange),
            Engine::Range<double>(0, m_line.a.z));

        Engine::AABB bBounds(Engine::Range<double>(m_line.b.x - bHorRange, m_line.b.x + bHorRange),
            Engine::Range<double>(m_line.b.y - bHorRange, m_line.b.y + bHorRange),
            Engine::Range<double>(0, m_line.b.z));

        return aBounds | bBounds;
    }
    double Distance(const Engine::Coordinate& p) const
    {
        Engine::Line bottomLine = Engine::Plane::xy.Project(m_line);
        // compute c: the height of the hill at p
        Engine::Coordinate c(p.x, p.y, 0);
        if (bottomLine.Length() > std::numeric_limits<double>::epsilon())
        {
            auto bottomProjection = bottomLine.Project(c);
            double distance = (bottomProjection - c).Length();
            double interpolation_factor = std::min(1.0, (c - bottomLine.a).Length() / bottomLine.Length());
            c.z = Engine::Lerp(double(m_line.a.z), double(m_line.b.z), interpolation_factor) * Engine::Gaussian(distance, m_stddev);
        }
        else
        {
            double distance = (bottomLine.a - c).Length();
            c.z = m_line.a.z * Engine::Gaussian(distance, m_stddev);
        }
        return p.z - c.z;
    }
private:
    Engine::Line m_line;
    double m_stddev;

};
void VoxelMap::Hill(const Engine::Line& ridgeLine, double stddev)
{
    auto ridgeLineMeters = ridgeLine;
    ridgeLineMeters.a.z *= voxels.GridSize().z;
    ridgeLineMeters.b.z *= voxels.GridSize().z;

    Game::Hill hill(ridgeLineMeters, stddev);

    auto dbgCount = voxels.Fill(hill, Material::stone, atmosphericTemperature);
    OutputDebugStringW((std::wstring(L"Hill at ") + Engine::ToWString(ridgeLine) + L"=" + std::to_wstring(dbgCount) + L" voxels\n").c_str());
}

void VoxelMap::Wall(const Engine::Line& bottomLine, double height, double thickness)
{
    height *= voxels.GridSize().z;   // from grids to meters
    Engine::Line bottomMeters = bottomLine;
    bottomMeters.a.z *= voxels.GridSize().z;
    bottomMeters.b.z *= voxels.GridSize().z;    // todo in a matrix and you know, better
    Engine::Box box(Engine::Vector(bottomMeters.Length(), thickness, height));
    auto xAxis = Engine::Vector(bottomMeters).Normal();
    Engine::Vector zAxis(0, 0, 1);
    // translate the box up so the bottomline starts at 0,0,0 first
    box *= Engine::Matrix::Translation(Engine::Vector(bottomMeters.Length()*0.5, 0, height*0.5));

    box *= Engine::Matrix(
        xAxis,  // the bottom line was in xAxis, now rotated towards the line
        zAxis.Cross(xAxis),  // the yAxis is perpendicular to the x and z simply
        zAxis,  // the zAxis remains aligned with the world to not rotate the wall, but skew it along the line 
        Engine::Vector(bottomLine.a));

    auto dbgCount = voxels.Fill(box, Material::stone, atmosphericTemperature);
    OutputDebugStringW((std::wstring(L"Wall at ") + Engine::ToWString(bottomLine) + L"=" + std::to_wstring(dbgCount)+L" voxels\n").c_str());

}

unsigned VoxelMap::Latitude() const
{
    return voxels.GetSize().x;
}
unsigned VoxelMap::Longitude() const
{
    return voxels.GetSize().y;
}

Directions VoxelMap::Visibility(const Position& p) const
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

Square VoxelMap::At(const Position& p) const
{
    auto position = p;
    auto mapBounds = voxels.Bounds().Grow(Size(-1, -1, 0));
    if (!mapBounds.Contains(position))
        return Square();
    for (int i = position.z; i >= 0; --i)
    {
        const auto& v = voxels[Position(position.x, position.y, i)];
        if (v.Solid())
        {
            if (v.temperature >= 500.0f)
            {
                return Square(Element::Fire, true, i + 1);
            }
            else
            {
                static const std::map<const Material*, Element> elementMap = {
                    { &Material::vacuum, Element::None },
                    { &Material::air, Element::Air },
                    { &Material::soil, Element::Nature},
                    { &Material::stone, Element::Stone },
                    { &Material::water, Element::Water }
                };
                return Square(elementMap.at(v.material), true, i + 1);
            }
        }
    }
    return Square(Element::Fire, true, 0);
}

/*
void VoxelMap::ComputeForces(double seconds)
{
    // Flow
    for (auto& v : voxels)
    {
        if (v.boundary)
        {
        }
        else
        {   // en.wikipedia.org/wiki/Viscosity#Dynamic_and_kinematic_viscosity
            double viscosity = v->Viscosity() / v->Density();
            v->flow -= v->flow * viscosity * seconds;
            auto pressure = v->Pressure();
            for (auto d : Direction::all)
            {
                auto neightbourPosition = v.position + d.Vector();
                // presssure (Pa) is N/m2
                auto& neighbour = voxels[neightbourPosition];
                if (!neighbour.Solid())
                {   
                    // Pressure gradient (Pa) is N/m^2. Times surface area is newton
                    double force = d.Surface() * (pressure - neighbour.Pressure());  // divide over distance?
                    // Law of motion. Acceleration = Force/mass (kg)
                    double acceleration = (force / (v->Mass()*1e-3));
                    //  multiplied with time it's the change in velocity
                    acceleration *= seconds;
                    Engine::Vector dir(d.Vector().x, d.Vector().y, d.Vector().z);
                    // Not newtonian, spread force over two grids
                    v->flow += dir * acceleration * 0.5;
                    double neighbourAcceleration = (force * seconds / neighbour.Mass()*1e-3);
                    neighbour.flow += dir * neighbourAcceleration *0.5;
                    if (d == Direction::down)
                    {
                        v->flow += Engine::Vector(0, 0, -gravity)*seconds;
                    }
                }
                else
                {   // normal force
                    Engine::Vector normal(-d.Vector().x, -d.Vector().y, -d.Vector().z);
                    auto dot = normal.Dot(v->flow);
                    if (dot < 0)
                    {   // TODO: check the force against the strength of the surface
                        v->flow -= normal * dot;
                    }
                }
            }
        }
    }
}
*/

/*
void VoxelMap::Diffuse(double seconds)
{

    const Data original = voxels;   // TODO can make more efficient partial copy
    auto it = original.begin();
    for (auto& v : voxels)
    {
        if (v.boundary)
        {   // Complete immediate diffusion 
            //  TODO: could do it over time and surface of boundary but then this wouldn't be the boundary
            v->temperature = AtmosphericTemperature(v.position.z);
            double pressure = AtmosphericPressure(v.position.z);
            v->density = float(v->material->Density(pressure, v->temperature));
        }
        else
        {
            for (auto d : Direction::all)
            {   
                const auto& old = **it;
                auto neighbourPosition = v.position + d.Vector();
                const auto& oldNeighbour = original[neighbourPosition];
                if (old.material == oldNeighbour.material)
                {
                    double coefficient = old.DiffusionCoefficient(oldNeighbour);
                    if (coefficient != 0)
                    {   // Simplified Diffusion equation: https://en.wikipedia.org/wiki/Diffusion_equation
                        // dDensity/dt = DiffusionFactor * density
                        //  since volume is (currently) equal between each voxel, dMass = DiffusionFactor * mass * dt
                        auto diffusionFactor = coefficient * seconds * d.Surface();
                        assert(diffusionFactor > 0);   // don't know what negative diffusion means 
                        auto deltaDensity = float((old.density - oldNeighbour.density) * diffusionFactor);
                        auto deltaTemperature = float((old.temperature - oldNeighbour.temperature) * diffusionFactor);

                        v->density -= deltaDensity;
                        // Thermal convenction: https://en.wikipedia.org/wiki/Thermal_conduction
                        v->temperature -= deltaTemperature;
                        if (!voxels.IsBoundary(neighbourPosition))
                        {
                            auto& neighbour = voxels[neighbourPosition];
                            neighbour.density += deltaDensity;
                            neighbour.temperature += deltaTemperature;
                        }
                    }
                }
                else
                {
                    // dont diffuse different materials yet
                    // Always diffuse heat
                    // https://en.wikipedia.org/wiki/Thermal_conduction#Fourier's_law
                    auto flux = v->material->conductivity * (old.temperature - oldNeighbour.temperature);
                    auto deltaJoule = flux * seconds * d.Surface();
                    auto& neighbour = voxels[neighbourPosition];
                    v->temperature -= float(deltaJoule / (old.material->heatCapacity*old.Mass()));
                    neighbour.temperature += float(deltaJoule / (oldNeighbour.material->heatCapacity*oldNeighbour.Mass()));
                }
            }
        }
        ++it;
    }
}
*/

/*

Position VoxelMap::MaxFlow() const
{
    double maxSqrFlow = 0;
    Position maxLocation;
    for (auto& v : voxels)
    {
        double sqr = v->flow.LengthSquared();
        if (sqr > maxSqrFlow)
        {
            maxSqrFlow = sqr;
            maxLocation = v.position;
        }
    }
    return maxLocation;
}
*/

unsigned VoxelMap::WindForce() const
{
    double speed = 0;
    for (auto v : voxels)
    {
        auto flow = voxels.Flow(v.first);
        speed = std::max(speed, flow.Length());
    }
    constexpr std::array<double, 12> beaufortScale = { 0.2, 1.5, 3.3, 5.4, 7.9, 10.7, 13.8, 17.1, 20.7, 24.4, 28.4, 32.6 };
    unsigned beaufort = 0;
    for (auto force : beaufortScale)
    {
        if (speed <= force)
            break;
        beaufort++;
    }
    return beaufort;
}

double VoxelMap::Volume() const
{
    return voxels.GetSize().Volume() * LiterPerVoxel;
}

double VoxelMap::Mass(const Material& material) const
{
    return std::accumulate(voxels.begin(), voxels.end(), 0.0, [&material](double runningTotal, const decltype(voxels)::value_type& v)
    {
        if (v.second.material == &material)
            return runningTotal + v.second.Mass();
        else
            return runningTotal;
    });
}

double VoxelMap::Temperature(const Material& material) const
{
    auto heat = std::accumulate(voxels.begin(), voxels.end(), 0.0, [&material](double runningTotal, const decltype(voxels)::value_type& v)
    {
        if (v.second.material == &material)
            return runningTotal + v.second.Mass() * v.second.temperature;
        else
            return runningTotal;
    });

    return heat / Mass(material);
}

double VoxelMap::Mass() const
{
    return std::accumulate(voxels.begin(), voxels.end(), 0.0, [](double runningTotal, const decltype(voxels)::value_type& v)
    {
        return runningTotal + v.second.Mass();
    });
}

/*
void VoxelMap::Advection(double seconds)
{

    const Data original = voxels;   
    auto it = original.begin();
    Engine::Vector maxFlow = voxels[MaxFlow()].flow * seconds;
    // need to divide in steps if advection is larger than grid size
    assert(maxFlow.x < HorizontalEl*MeterPerEl);    // should be good to 100m/s
    assert(maxFlow.y < HorizontalEl*MeterPerEl);
    assert(maxFlow.z < VerticalEl*MeterPerEl);      // this may be a problem with explosions

    for (auto& org : original)
    {
        if (org.boundary)
        {   
        }
        else
        {
            // Axis aligned bounding box of the destination volume
            //  relative to the original's position
            auto displacement = org->flow*seconds;
            Engine::AABB newBB(Engine::Coordinate(displacement.x, displacement.y, displacement.z),
                Engine::Vector(1, 1, 1));

            auto& source = voxels[org.position];
            for (int x = -1; x <= 1; ++x)
            {
                for (int y = -1; y <= 1; ++y)
                {
                    for (int z = -1; z <= 1; ++z)
                    {
                        Position targetPosition(org.position.x + x, org.position.y + y, org.position.z + z);
                        auto& target = voxels[targetPosition];
                                
                        if (target.material == org->material)
                        {
                            // Axis aligned bounding box of the target volume, also relative
                            Engine::AABB volumeBB(Engine::Coordinate(x, y, z), Engine::Vector(1, 1, 1));
                            Engine::AABB overlap = newBB & volumeBB;
                            auto overlapVolume = overlap.Volume();
                            if (overlapVolume > 0)
                            {
                                target.density += float(overlapVolume * org->density);
                                target.temperature += float(overlapVolume * org->temperature);
                                target.flow += org->flow * overlapVolume;

                                source.density -= float(org->density * overlapVolume);
                                source.temperature -= float(org->temperature * overlapVolume);
                                source.flow -= org->flow * overlapVolume;
                            }
                        }
                    }
                }
            }
        }
    }
}
*/


void VoxelMap::FluxBoundary()
{

    for (auto dir : Direction::all)
    {
        FluxBoundary(Directions(dir));
        for (auto axis : Directions::axes)
        {
            if (dir.IsParallel(axis))
                continue;
            // edge
            FluxBoundary(dir | axis);
            FluxBoundary(dir | axis.Opposite());
            // corners
            auto perpendicular = dir.Perpendicular(axis);
            FluxBoundary(dir | axis | perpendicular);
            FluxBoundary(dir | axis.Opposite() | perpendicular);
            FluxBoundary(dir | axis | perpendicular.Opposite());
            FluxBoundary(dir | axis.Opposite() | perpendicular.Opposite());
        }
    }
}

void VoxelMap::FluxBoundary(Directions boundary)
{
    // too slow to do it for every voxel? 
    Engine::Vector flowVolume = wind * Material::air.Density(AtmosphericPressure(0), AtmosphericTemperature(0));
    Engine::Vector windFlux(
        flowVolume.x / Direction::east.Surface(),
        flowVolume.y / Direction::north.Surface(),
        flowVolume.z / Direction::up.Surface()
    );
    for (auto u : voxels.U().BoundaryCondition(boundary))
    {
        auto boundaryPosition = u.first;
        if (&voxels.MaterialAt(boundaryPosition) == &Material::stone)
            voxels.U().SetBoundary(u.first, boundary, 0);  // TODO: should be any solid and except for deliberate or z=0 lava flows ...
        else if (&voxels.MaterialAt(boundaryPosition) == &Material::air)
            voxels.U().SetBoundary(u.first, boundary, windFlux.x);
        else
            assert(false);  // water boundary condition has to be defined by rivers and seas

    }
    for (auto v : voxels.V().BoundaryCondition(boundary))
    {
        auto boundaryPosition = v.first;
        if (&voxels.MaterialAt(boundaryPosition) == &Material::stone)
            voxels.V().SetBoundary(v.first, boundary, 0);
        else if (&voxels.MaterialAt(boundaryPosition) == &Material::air)
            voxels.V().SetBoundary(v.first, boundary, windFlux.y);
        else
            assert(false);  
    }
    for (auto w : voxels.W().BoundaryCondition(boundary))
    {
        auto boundaryPosition = w.first;
        if (&voxels.MaterialAt(boundaryPosition) == &Material::stone)
            voxels.W().SetBoundary(w.first, boundary, 0);
        else if (&voxels.MaterialAt(boundaryPosition) == &Material::air)
            voxels.W().SetBoundary(w.first, boundary, windFlux.z);
        else
            assert(false);
    }
}

void VoxelMap::GridBoundary()
{
    for (auto d : Direction::all)
    {
        for (auto v : voxels.BoundaryCondition(d))
        {
            // Material of the boundary grid is irrelevant. If it's a different material, the density gradient 
            //  will be very high at the boundary, but since this is not registered we just extrapolate
            auto realPosition = v.first - d.Vector();
            const Voxel realWorld = voxels[realPosition];
            float boundaryTemperature = AtmosphericTemperature(Elevation(v.first.z));
            float boundaryDensity = float(realWorld.material->Density(AtmosphericPressure(Elevation(v.first.z)), boundaryTemperature));

            // extrapolate to set
            voxels.AdjustGrid(v.first, 
                Engine::Lerp(realWorld.temperature, boundaryTemperature, 2.0),
                Engine::Lerp(realWorld.density, boundaryDensity, 2.0));
        }
    }
}

void VoxelMap::Flow(double dt)
{
    const Data::Flux oU = voxels.U();
    const Data::Flux oV = voxels.V();
    const Data::Flux oW = voxels.W();
    constexpr double dx = HorizontalEl * MeterPerEl;
    constexpr double dy = HorizontalEl * MeterPerEl;
    constexpr double dz = VerticalEl * MeterPerEl;
    // U Flow
    double Reynolds = 100.0;    // TODO: Apparently has to do with viscosity and friction https://en.wikipedia.org/wiki/Reynolds_number
    for (const auto& uit : oU)
    {
        // d  ab d something: 
        //  first line before - : for B values, +1 their coordinate
        //  second line after - : remove -1 
        // dpd something: +1 to +0
        const Position& p = uit.first;
        double duudx = (Engine::Sqr(oU(p.x + 1, p.y, p.z)) - Engine::Sqr(oU(p.x - 1, p.y, p.z))) / (2.0*dx);
        double duvdy = 0.25*((oU(p.x, p.y, p.z) + oU(p.x, p.y + 1, p.z)) * (oV(p.x, p.y, p.z) + oV(p.x + 1, p.y, p.z))
            - (oU(p.x, p.y, p.z) + oU(p.x, p.y - 1, p.z)) * (oV(p.x, p.y - 1, p.z) + oV(p.x + 1, p.y - 1, p.z))) / dy;
        double duwdz = 0.25*((oU(p.x, p.y, p.z) + oU(p.x, p.y, p.z + 1)) * (oW(p.x, p.y, p.z) + oW(p.x + 1, p.y, p.z))
            - (oU(p.x, p.y, p.z) + oU(p.x, p.y, p.z - 1)) * (oW(p.x, p.y, p.z - 1) + oW(p.x + 1, p.y, p.z - 1))) / dz;
        double dpdx = (voxels.VoxelData::Density(Position(p.x + 1, p.y, p.z)) - voxels.VoxelData::Density(p)) / dx;
        // can be computed from visocity and flow speed, but also need to know how far to the nearest surface 
        // it should be as simply as massflowrate (=flux*area) / diameter * dynamic viscosity of the material
        // trick will be to compute the diamter in x y and z for each grid
        double diff = (1.0 / Reynolds) *
            ((oU(p.x + 1, p.y, p.z) - 2.0*oU(p.x, p.y, p.z) + oU(p.x - 1, p.y, p.z)) / (dx*dx) +
            (oU(p.x, p.y + 1, p.z) + 2.0*oU(p.x, p.y, p.z) + oU(p.x, p.y - 1, p.z)) / (dy*dy) +
                (oU(p.x, p.y, p.z + 1) + 2.0*oU(p.x, p.y, p.z) + oU(p.x, p.y, p.z - 1)) / (dz*dz));
        voxels.U()[p] += float(dt * (diff - duudx - duvdy - duwdz - dpdx));
    }
    // V Flow

    for (const auto& vit : oV)
    {
        const Position& p = vit.first;
        double dvvdy = (Engine::Sqr(oV(p.x, p.y + 1, p.z)) - Engine::Sqr(oV(p.x, p.y - 1, p.z))) / (2.0*dy);
        double dvudx = 0.25*((oV(p.x, p.y, p.z) + oV(p.x + 1, p.y, p.z)) * (oU(p.x, p.y, p.z) + oU(p.x, p.y + 1, p.z))
            - (oV(p.x, p.y, p.z) + oV(p.x - 1, p.y, p.z)) * (oU(p.x - 1, p.y, p.z) + oU(p.x - 1, p.y + 1, p.z))) / dx;
        double dvwdz = 0.25*((oV(p.x, p.y, p.z) + oV(p.x, p.y, p.z + 1)) * (oW(p.x, p.y, p.z) + oW(p.x, p.y + 1, p.z))
            - (oV(p.x, p.y, p.z) + oV(p.x, p.y, p.z - 1)) * (oW(p.x, p.y, p.z - 1) + oW(p.x, p.y + 1, p.z - 1))) / dz;
        double dpdy = (voxels.VoxelData::Density(Position(p.x, p.y + 1, p.z)) - voxels.VoxelData::Density(p)) / dy;
        // TODO: code duplication with u and z flow, move to flux 
        //  after unit test!
        double diff = (1.0 / Reynolds) *
            ((oV(p.x + 1, p.y, p.z) - 2.0*oV(p.x, p.y, p.z) + oV(p.x - 1, p.y, p.z)) / (dx*dx) +
            (oV(p.x, p.y + 1, p.z) + 2.0*oV(p.x, p.y, p.z) + oV(p.x, p.y - 1, p.z)) / (dy*dy) +
                (oV(p.x, p.y, p.z + 1) + 2.0*oV(p.x, p.y, p.z) + oV(p.x, p.y, p.z - 1)) / (dz*dz));
        voxels.V()[p] += float(dt * (diff - dvvdy - dvudx - dvwdz - dpdy));
    }
    // W Flow
    for (const auto& wit : oW)
    {
        const Position& p = wit.first;
        double dwwdz = (Engine::Sqr(oW(p.x, p.y, p.z+1)) - Engine::Sqr(oW(p.x, p.y, p.z-1))) / (2.0*dz);
        double dwvdy = 0.25*((oW(p.x, p.y, p.z) + oW(p.x, p.y + 1, p.z)) * (oV(p.x, p.y, p.z) + oV(p.x, p.y, p.z + 1))
            - (oW(p.x, p.y, p.z) + oW(p.x, p.y - 1, p.z)) * (oV(p.x, p.y - 1, p.z) + oV(p.x, p.y - 1, p.z + 1))) / dy;
        double dwudx = 0.25*((oW(p.x, p.y, p.z) + oW(p.x, p.y, p.z + 1)) * (oU(p.x, p.y, p.z) + oU(p.x, p.y, p.z + 1))
            - (oW(p.x, p.y, p.z) + oW(p.x - 1, p.y, p.z)) * (oU(p.x - 1, p.y, p.z ) + oU(p.x - 1, p.y, p.z + 1))) / dx;
        double dpdz = (voxels.VoxelData::Density(Position(p.x, p.y, p.z + 1)) - voxels.VoxelData::Density(p)) / dz;
        double diff = (1.0 / Reynolds) *
            ((oW(p.x + 1, p.y, p.z) - 2.0*oW(p.x, p.y, p.z) + oW(p.x - 1, p.y, p.z)) / (dx*dx) +
            (oW(p.x, p.y + 1, p.z) + 2.0*oW(p.x, p.y, p.z) + oW(p.x, p.y - 1, p.z)) / (dy*dy) +
                (oW(p.x, p.y, p.z + 1) + 2.0*oW(p.x, p.y, p.z) + oW(p.x, p.y, p.z - 1)) / (dz*dz));
        voxels.W()[p] += float(dt * (diff - dwwdz - dwvdy - dwudx - dpdz));
    }

    FluxBoundary();
}

void VoxelMap::Continuity(double seconds)
{   
    for (auto it = voxels.begin(); it!=voxels.end(); ++it)
    {
        auto fluxGradient = voxels.FluxGradient(it.position);
        auto p = voxels.VoxelData::Density(it.position);
        auto dP = fluxGradient.x * fluxGradient.y * fluxGradient.z * seconds;
        voxels.SetDensity(it.position, float(p - dP));
    }
    GridBoundary();
}

void VoxelMap::Tick(double seconds)
{
    //if (!mesh)
    //    GenerateMesh();

    // Fluid dynamics:
    //  https://en.wikipedia.org/wiki/Fluid_dynamics
    //  With incompressible flow, as long as it stays under 111m/s (mach 0.3) is simpler
 
    Engine::Timer start;
    Flow(seconds);
    Continuity(seconds);
   
    time += seconds;
    double performance = start.Seconds();

    OutputDebugStringW((std::wstring(L"Tick in ") + std::to_wstring(performance*1000.0)+L"ms " 
        L"Wind=" + std::to_wstring(WindForce()) + L" bft. "+
        L"Mass=" + std::to_wstring(Mass()) + L"g " +
        L"Air Temp=" + std::to_wstring(int(Temperature(Material::air)-273.15)) + L"°C "+
        L"\n").c_str());

    SetDensityToMeshColor();
}

void VoxelMap::SetDensityToMeshColor()
{
    if (!mesh)
        return;
    mesh->Invalidate();
    for (auto& vertex : mesh->vertices)
    {
        Engine::Coordinate meters(vertex.c.x, vertex.c.z, vertex.c.y);
        auto position = voxels.Grid(meters);
        auto density = voxels.Density(meters);
        auto temperature = voxels.Temperature(meters);
        const auto& material = voxels.MaterialAt(position);
        double sigmoidDensity = Engine::Sigmoid((density - material.normalDensity)/material.normalDensity);
        double sigmoidTemperature = Engine::Sigmoid(temperature - atmosphericTemperature);
        vertex.color = Engine::RGBA(BYTE(127 + sigmoidTemperature * 127.0),
                    BYTE(127 + sigmoidDensity * 127.0),
                    BYTE(127 - sigmoidDensity * 127.0), 255);
    }
}



void VoxelMap::GenerateMesh()
{
    Engine::Vector grid = voxels.GridSize();
    mesh = std::make_unique<Engine::Mesh>();
    for (auto& v : voxels)
    {
        auto c = v.second.Color();
        if (!c)
            continue;
        auto visible = Visibility(v.first);
        if (!visible)
            continue;

        // TODO: matrix from position to gl, swapping y and z, translation and grid scaling

        Engine::Box box(Engine::Vector(grid.x, grid.z, grid.y));    // nb z<=>y
        box.SetColor(c);

        box *= Engine::Matrix::Translation(
            Engine::Vector((double(v.first.x) + 0.5)*grid.x,
            (double(v.first.z) + 0.5)*grid.z,
                (double(v.first.y) + 0.5)*grid.y));
        (*mesh) += box;
    }
}

void VoxelMap::Render() const
{
    if (!mesh)
        const_cast<VoxelMap*>(this)->GenerateMesh();
    if (mesh)
    {
        glEnable(GL_LIGHTING);
        glDisable(GL_BLEND);
        mesh->Render();
        glDisable(GL_LIGHTING);
        glEnable(GL_BLEND);
        mesh->Render();
    }
    else
    {
        //RenderPretty();
        RenderAnalysis();
    }
}

void VoxelMap::RenderPretty() const
{
    /*
    assert(glIsEnabled(GL_DEPTH_TEST));

    // Draw opaque
    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
    for (auto& v : voxels)
    {
        glPushMatrix();
        glTranslated(v.first.x, v.first.z*MeterPerEl, v.first.y); // offset by -1,-1,-1 for boundary
        if (v.second.Opaque())
        {
            if (auto visibility = Visibility(v.first))
            {
                v.second.Render(v.first, visibility);
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
        glTranslated(v.first.x, v.first.z*MeterPerEl, v.first.y); // offset by -1,-1,-1 for boundary
        if (!v.second.Opaque() && !v.second.Transparent())
        {
            auto visibility = Visibility(v.first);
            if (!visibility.empty())
            {
                v.second.Render(v.first, visibility);
            }
        }
        glPopMatrix();
    }
    Engine::CheckGLError();
    */
}


void VoxelMap::RenderAnalysis() const
{
    /*
    assert(glIsEnabled(GL_DEPTH_TEST));

    // Draw opaque
    glDisable(GL_BLEND);
    glDisable(GL_LIGHTING);
    for (const auto v : voxels)
    {
        glPushMatrix();
        glTranslated(v.first.x, v.first.z*MeterPerEl, v.first.y);
        v.second.RenderAnalysis(v.first, Directions::all, voxels.DensityGradient(v.first));
        glPopMatrix();
    }


    glColor3d(1, 1, 1);
    Position p;
    for (auto u : voxels.U())
    {
        glPushMatrix();
        glTranslated(u.first.x, (u.first.z + 0.5)*MeterPerEl, u.first.y + 0.5);
        Engine::glDrawArrow(Engine::Vector(u.second, 0, 0));
        glPopMatrix();
    }
    for (auto v : voxels.V())
    {
        glPushMatrix();
        glTranslated(v.first.x + 0.5, (v.first.z + 0.5)*MeterPerEl, v.first.y);
        Engine::glDrawArrow(Engine::Vector(0, 0, v.second));
        glPopMatrix();
    }
    for (auto w : voxels.W())
    {
        glPushMatrix();
        glTranslated(w.first.x + 0.5, w.first.z*MeterPerEl, w.first.y + 0.5);
        Engine::glDrawArrow(Engine::Vector(0, w.second, 0));
        glPopMatrix();
    }
    glColor3d(0.5, 0.5, 0.5);
    for (auto d : Direction::all)
    {
        for (auto u : voxels.U().BoundaryCondition(Directions(d)))
        {
            double v = u.first;
            glPushMatrix();
            glTranslated(u.first.x, (u.first.z + 0.5)*MeterPerEl, u.first.y + 0.5);
            Engine::glDrawArrow(Engine::Vector(u.second, 0, 0));
            glPopMatrix();
        }
        for (auto v : voxels.V().BoundaryCondition(Directions(d)))
        {
            glPushMatrix();
            glTranslated(v.first.x + 0.5, (v.first.z + 0.5)*MeterPerEl, v.first.y);
            Engine::glDrawArrow(Engine::Vector(0, 0, v.second));
            glPopMatrix();
        }
        for (auto w : voxels.W().BoundaryCondition(Directions(d)))
        {
            glPushMatrix();
            glTranslated(w.first.x + 0.5, w.first.z*MeterPerEl, w.first.y + 0.5);
            Engine::glDrawArrow(Engine::Vector(0, w.second, 0));
            glPopMatrix();
        }
    }
/*
    for (auto d : Direction::all)
    {
        for (auto v : voxels.BoundaryCondition(Directions(d)))
        {
            glPushMatrix();
            glTranslated(v.position.x, v.position.z*MeterPerEl, v.position.y);
            v.Render(v.position, Directions(0xFFFF));
            glPopMatrix();
        }
    }
*/
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
    map.Sea(waterLevel, temperature);
    map.Air(temperature, 20000);
    unsigned procedures;
    s >> procedures;
    for (unsigned p = 0; p < procedures; ++p)
    {
        std::wstring procedure;
        s >> procedure;
        if (procedure == L"WIND")
        {
            Engine::Vector wind;
            s >> wind;
            map.Wind(wind);
        }
        else if (procedure == L"WALL")
        {
            Engine::Coordinate p0, p1;
            float height, width;
            s >> p0 >> p1 >> height >> width;
            map.Wall(Engine::Line(p0, p1), height, width);
        }
        else if (procedure == L"HILL")
        {
            Engine::Coordinate p0, p1;
            float stddev;
            s >> p0 >> p1 >> stddev;
            map.Hill(Engine::Line(p0, p1), stddev);
        }
        else
        {
            throw std::runtime_error("Unknown procedure");
        }
    }
    return s;
}

}
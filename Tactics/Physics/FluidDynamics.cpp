#include "stdafx.h"
#include <numeric>
#include "FluidDynamics.h"
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

// Navier Stokes Explanation on youtube: 
// https://www.youtube.com/playlist?list=PLt5AfwLFPxWK_BKKR2xTZoh2MddOtGbAM

namespace Physics
{

FluidDynamics::FluidDynamics(const Engine::Vector& size, const Engine::Vector& grid) :
    voxels(Size(int(std::round(size.x / grid.x)), int(std::round(size.y / grid.y)), int(std::round(size.z / grid.z))), grid)
{
}

FluidDynamics::~FluidDynamics() = default;

/*
float FluidDynamics::AtmosphericTemperature(double elevation) const
{
    double atmosphericLapse = atmosphericTemperature / atmosphereRadius;
    return float(atmosphericTemperature + atmosphericLapse * elevation);
}

float FluidDynamics::AtmosphericPressure(double elevation) const
{
    return float(PascalPerAtmosphere * (1.0- elevation/ atmosphereRadius));
}

*/

Directions FluidDynamics::Visibility(const Position& p) const
{
    Directions result;
    for (const Direction& direction : Direction::all)
    {
        auto neighbourPosition = p + direction.Vector();

        if (!voxels.IsInside(neighbourPosition) || 
            !voxels[neighbourPosition].Opaque(direction.Vector().Size()))
        {
            result|=direction;
        }
    }
    return result;
}

/*
void FluidDynamics::ComputeForces(double seconds)
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
void FluidDynamics::Diffuse(double seconds)
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

Position FluidDynamics::MaxFlow() const
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

unsigned FluidDynamics::WindForce() const
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

double FluidDynamics::Volume() const
{
    auto gs = voxels.GridSize();
    return voxels.GetSize().Volume() * voxels.VoxelVolume();
}

double FluidDynamics::Mass(const Material& material) const
{
    auto vol = voxels.VoxelVolume();
    return std::accumulate(voxels.begin(), voxels.end(), 0.0, [&material, vol](double runningTotal, const decltype(voxels)::value_type& v)
    {
        if (v.second.material == &material)
            return runningTotal + v.second.Mass(vol);
        else
            return runningTotal;
    });
}

double FluidDynamics::Temperature(const Material& material) const
{
    auto vol = voxels.VoxelVolume();
    auto heat = std::accumulate(voxels.begin(), voxels.end(), 0.0, [&material, vol](double runningTotal, const decltype(voxels)::value_type& v)
    {
        if (v.second.material == &material)
            return runningTotal + v.second.Mass(vol) * v.second.temperature;
        else
            return runningTotal;
    });

    return heat / Mass(material);
}

double FluidDynamics::Mass() const
{
    double vol = voxels.VoxelVolume();
    return std::accumulate(voxels.begin(), voxels.end(), 0.0, [vol](double runningTotal, const decltype(voxels)::value_type& v)
    {
        return runningTotal + v.second.Mass(vol);
    });
}

/*
void FluidDynamics::Advection(double seconds)
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


void FluidDynamics::FluxBoundary()
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

void FluidDynamics::FluxBoundary(Directions boundary)
{
    // too slow to do it for every voxel? 
    Engine::Vector flowVolume = wind * Material::air.Density(AtmosphericPressure(0), AtmosphericTemperature(0));
    Engine::Vector windFlux(
        flowVolume.x / Direction::east.Surface(voxels.GridSize()),
        flowVolume.y / Direction::north.Surface(voxels.GridSize()),
        flowVolume.z / Direction::up.Surface(voxels.GridSize())
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

void FluidDynamics::GridBoundary()
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

void FluidDynamics::Flow(double dt)
{
    const Data::Flux oU = voxels.U();
    const Data::Flux oV = voxels.V();
    const Data::Flux oW = voxels.W();
    const double dx = voxels.GridSize().x;
    const double dy = voxels.GridSize().y;
    const double dz = voxels.GridSize().z;
    // U Flow
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
        double densityA = voxels.VoxelData::Density(p);
        double densityB = voxels.VoxelData::Density(Position(p.x + 1, p.y, p.z));
        double dpdx = (densityB - densityA) / dx;
        // TODO: feed th velocity here (should be sort of flux (material/second) divided by density (material/space) and the other directions
        double Reynolds = voxels.VoxelData::MaterialAt(p).Reynolds(voxels.VoxelData::Temperature(p), (densityA+densityB)*0.5);
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
        auto densityA = voxels.VoxelData::Density(p);
        auto densityB = voxels.VoxelData::Density(Position(p.x, p.y + 1, p.z));
        double dpdy = (densityB- densityA) / dy;
        // TODO: code duplication with u and z flow, move to flux 
        //  after unit test!
        double Reynolds = voxels.VoxelData::MaterialAt(p).Reynolds(voxels.VoxelData::Temperature(p), (densityA+densityB)*0.5);
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
        auto densityA = voxels.VoxelData::Density(p);
        auto densityB = voxels.VoxelData::Density(Position(p.x, p.y, p.z + 1));
        double dpdz = (densityB - densityA) / dz;
        double Reynolds = voxels.VoxelData::MaterialAt(p).Reynolds(voxels.VoxelData::Temperature(p), (densityA+densityB)*0.5);
        double diff = (1.0 / Reynolds) *
            ((oW(p.x + 1, p.y, p.z) - 2.0*oW(p.x, p.y, p.z) + oW(p.x - 1, p.y, p.z)) / (dx*dx) +
            (oW(p.x, p.y + 1, p.z) + 2.0*oW(p.x, p.y, p.z) + oW(p.x, p.y - 1, p.z)) / (dy*dy) +
                (oW(p.x, p.y, p.z + 1) + 2.0*oW(p.x, p.y, p.z) + oW(p.x, p.y, p.z - 1)) / (dz*dz));
        voxels.W()[p] += float(dt * (diff - dwwdz - dwvdy - dwudx - dpdz));
    }

    FluxBoundary();
}

void FluidDynamics::Continuity(double seconds)
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

void FluidDynamics::Tick(double seconds)
{

    // Fluid dynamics:
    //  https://en.wikipedia.org/wiki/Fluid_dynamics
    //  With incompressible flow, as long as it stays under 111m/s (mach 0.3) is simpler
 
    Engine::Timer start;
    Flow(seconds);
    Continuity(seconds);
   
    time += seconds;
    double performance = start.Seconds();
}

}
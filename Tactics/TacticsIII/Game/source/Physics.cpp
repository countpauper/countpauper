#include "Game/Physics.h"
#include "Game/Stack.h"
#include <ranges>
namespace Game 
{

Physics::Physics(float gravity) :
    gravity(gravity)
{
}

void Physics::Pressure(Cell& first, Cell::Height firstHeight, Cell& second, Cell::Height secondHeight, float area, Orientation axis, float dt) const
{
    // gravity is taken into account in the static pressure gradient of each cell 
    auto firstPressure =  first.GetPressure(firstHeight, gravity);
    auto secondPressure = second.GetPressure(secondHeight, gravity);
    // TODO gravity is already taken into account in the static pressure for non solids 
    // This means a difference of no pressure means no force. 
    // If there is a force, then multiply with the mass of the cell that it is towards to increase that pressure 
    // if the desination is compressible it should shrink to achieve this. else what ? Solids may crack. Liquids will just push on 
    auto deltaPressure =  firstPressure - secondPressure;   // Pa * 1m2
    // TODO the distance depends on the type of interface. It may be a lot less with 
    // 1) surface tension 
    // 2) incompressible target
    // 3) differene that is not because of hydrostatic pressure, but sharper interface due to temperature or density
    float distance = 1.0; 
    if (axis.IsHorizontal())
    {
        distance = static_cast<float>((first.height + seocnd.height) / 2);
    }

    if (deltaPressure > 0) 
    {
        // TODO: the distance is not the same for all types of interfaces. This (cell to cell center) is basically the case for the same gas fully advecting
        // Instead compute opposing forces: 
        // 1) density increase from incompressible 
        // 2) surface tension (may be insignificant). This reduces the amount of mass that is lost by spraying (as the tension)
        // 3) dynamic viscostiy already 
        // 4) inellastic transfer of the force through or into a solid (if there is no transfer then crack )
        // At the same time this should still result in internal acceleration (up to the interface) which moves objects 
        float acceleration = deltaPressure / (first.Density() * distance); // N/m2 / kg/m3 = N/m2 / Nm2s2 =  
        if (second.IsCompressible())
        {
            first.AccelerateFlow(axis, acceleration * dt);
    }
    else 
    {
        float acceleration = deltaPressure / (second.Density() * distance); // kg/m3 * N/m2 = N/m3 = kg/m2/s2
        if (first.IsCompressible())
        {
            first.AccelerateFlow(axis, acceleration * dt);
        }
    }

}

Vertical::Vertical(float gravity) :
    Physics(gravity)
{
}
void Gravity::operator()(float dt, Stack& stack) const
{
    float dv = dt * gravity; // meter per second
    static Cell bottom(Material::bedrock, 255.95, 273.0);
    static Cell top(Material::air, 255.95, 273.0, 101000.0);    // TODO if rock (cave) then top is also rock to prevent dropping? 
    Cell& previous = top;  
    for(auto& cell: std::ranges::reverse_view(stack))
    {
        Pressure(cell, 0, previous, previous.height, 1.0, dt);
        previous = cell;
    }
    Pressure(previous, bottom, dt);
}

Viscosity::Viscosity() = default;

void Viscosity::operator()(float dt, Stack& stack) const 
{
    for(auto& cell: stack)
    {
        // TODO: this is a simplified/optimized viscosity model with simply exponential dampening
        // for a more accurate model compute the "shear stress" as a Friction Force and decelerate a = F/m 
        // This shear stress is caused by relative velocity with another material, which would be the neihgbours. 
        // These could be stationary if solid or the flow along the same axis of the cell flowing into and/or along.
        // Since this is based on velocity and then affects velocity, RungeKutta could be needed to improve accuracy 
        // It also needs the two neighbours along the positive axes (assuming the negative axes neigbours handle the flows in their direction).
        // (Supposedly) the computation would be: d
        // v_dx[axis] (neighbor.velocity[axis] - cell.velocity[axis]) / size[axis];
        // tau = mu * dv_dx (see wiki for tau and mu https://en.wikipedia.org/wiki/Viscosity)
        // F_shear = tau * (size[perpendicular axis]*size[other perpendicular axis]) // friction surface=interface with the neighbour. Could need to sum with multiple neighbours 
        // a = F / mass // deceleration
        // flow[axis] += a*dt 
        // Since flowing into cells, by definition has the same flow at the interface this is only done along perpendicular neighbours
        // It could also be possible to look one cell (C) further even and take that flow to handle clashing flows [A] -> [B] <- [C] but bounderies become painful  
        // Internal dampening with the exponent below can be kept to acocunt for internal friction 
        // Perhaps shear stress should be a separate physics function because it needs the neighbours. 
        if (cell.IsSolid())
            continue;       // solid layers don't "flow" but they still move
        auto mu = cell.Viscosity();
        float dampening = std::exp(-mu * dt / static_cast<float>(cell.Density()));
        for(auto& flow : cell.flow)
        {
            flow *= dampening;
        }
    }

}

}
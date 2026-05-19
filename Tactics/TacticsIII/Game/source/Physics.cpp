#include "Game/Physics.h"
#include "Game/Slice.h"

namespace Game 
{

Gravity::Gravity(float gravity) :
    gravity(gravity)
{
}

void Gravity::operator()(float dt, Slice& slice) const
{
    float dv = dt * gravity; // meter per second
    bool previousCompressible = true;
    for(auto& layer: slice)
    {
        if (previousCompressible)
            // TODO these should not be negative flow. Instead negative axes flow should be applied to the neighbour as an influx
            layer.AddFlow(Orientation::down, dv);
        previousCompressible = layer.IsCompressible();          
    }
}

Viscosity::Viscosity() = default;

void Viscosity::operator()(float dt, Slice& slice) const 
{
    for(auto& layer: slice)
    {
        // TODO: this is a simplified/optimized viscosity model with simply exponential dampening
        // for a more accurate model compute the "shear stress" as a Friction Force and decelerate a = F/m 
        // This shear stress is caused by relative velocity with another material, which would be the neihgbours. 
        // These could be stationay if solid or the flow along the same axis of the cell flowing into and/or along.
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
        if (layer.IsSolid())
            continue;       // solid layers don't "flow" but they still move
        auto mu = layer.Viscosity();
        float dampening = std::exp(-mu * dt / static_cast<float>(layer.density));
        for(auto& flow : layer.flow)
        {
            flow *= dampening;
        }
    }

}

}
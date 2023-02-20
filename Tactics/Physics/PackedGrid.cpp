#include "stdafx.h"
#include "PackedGrid.h"
#include "Direction.h"

namespace Physics
{

void PackedGrid::Tick(double seconds)
{
    Box invalid;
    Box bounds = Bounds();

    time += seconds;

    for (const auto c : constraints)
    {
        c.Tick(time, *this);
    }

    for (iterator it(*this, bounds); it != it.end(); ++it)
    {
        auto& current = (*it).second;

        invalid |= Heat(seconds, it.position, current);
        invalid |= Flow(it.position, current);
        invalid |= Evaporate(seconds, it.position, current);
    }
    effects.Tick(seconds);
    if (invalid)
        changed(grid.BoundingBox(invalid));
}


Box PackedGrid::Flow(const Position &position, PackedVoxel& current)
{
    Box invalid;
    invalid |= FlowDown(position, current);
    invalid |= FlowSide(position, current);
    invalid |= FlowUp(position, current);
    return invalid;
}

Box PackedGrid::FlowDown(const Position &position, PackedVoxel& current)
{
    Box invalid;
    if (!current.IsFluid())
        return invalid;

    Box bounds = Bounds();
    auto down = position + Direction::down.Vector();
    if (!bounds.Contains(down))
        return invalid;

    auto& neighbour = (*this)[down];
    auto newFill = 0;
    if (neighbour.IsGas())
    {
        newFill = 1;
    }
    else if ((neighbour.GetMaterial() == current.GetMaterial()) && (!neighbour.IsSolid()))
    {
        newFill = neighbour.Amount() + 1;
    }
    if ((newFill > 0) && (newFill < PackedVoxel::normalAmount))
    {
        invalid |= position;
        invalid |= down;
        neighbour.Set(current.GetMaterial(), newFill);

        double lerpFactor = 1.0 / neighbour.Amount();  // heat transfered by amount
        double newTemperature = Engine::Lerp(neighbour.Temperature(), current.Temperature(), lerpFactor);
        neighbour.SetTemperature(newTemperature);
        if (auto newAmount = current.Amount() - 1)
        {
            current.Set(current.GetMaterial(), newAmount);
        }
        else
        {
            current.Set(Material::air);
        }
    }
    return invalid;
}

Box PackedGrid::FlowSide(const Position &position, PackedVoxel& current)
{
    Box invalid;
    if (!current.IsFluid())
        return invalid;
    Box bounds = Bounds();

    // Flow horizontal, first make a list of options
    // which is neighbours with equal the least amount
    int drainAmount = current.Amount() - 1;
    if (drainAmount)
    {   // If the water level >=2 it can flow sideways to lower levels or air. Level 1 just waits to evaporate
        // find flow destinations
        const Directions sides = Direction::north | Direction::south | Direction::east | Direction::west;
        std::vector<Position> flowDestination;
        for (auto dir : sides)
        {
            auto side = position + dir.Vector();
            if (!bounds.Contains(side))
                continue;
            auto& neighbour = (*this)[side];
            if (neighbour.IsGas())
            {
                if (drainAmount > 0)
                    flowDestination.clear();
                drainAmount = 0;
                flowDestination.push_back(side);
            }
            else if ((neighbour.GetMaterial() == current.GetMaterial()) && (!neighbour.IsSolid()))
            {
                if (neighbour.Amount() < drainAmount)
                    flowDestination.clear();
                if (neighbour.Amount() <= drainAmount)
                {
                    drainAmount = neighbour.Amount();
                    flowDestination.push_back(side);
                }
            }
        }
        // Now spread the current amount over the neighbours
        while ((!flowDestination.empty()) && (current.Amount() > drainAmount))
        {
            // TODO: engine controlled random
            int idx = rand() % flowDestination.size();

            auto flowLocation = flowDestination[idx];
            flowDestination.erase(flowDestination.begin() + idx);
            invalid |= position;
            invalid |= flowLocation;
            if (auto newAmount = current.Amount() - 1)
                current.Set(current.GetMaterial(), newAmount);
            else
            {
                current.Set(Material::air);
            }
            auto& neighbour = (*this)[flowLocation];
            neighbour.Set(current.GetMaterial(), drainAmount + 1);

            double lerpFactor = 1.0 / neighbour.Amount();  // heat transfered by amount
            double newTemperature = Engine::Lerp(neighbour.Temperature(), current.Temperature(), lerpFactor);
            neighbour.SetTemperature(newTemperature);
        }
    }
    return invalid;
}

Box PackedGrid::FlowUp(const Position &position, PackedVoxel& current)
{
    Box invalid;
    if (!current.IsFluid())
        return invalid;
    // if water > normal pressure (atmospheric) easily checked with normal amount, potentially flow up
    if (current.Amount() <= PackedVoxel::normalAmount)
        return invalid;

    auto up = position + Direction::up.Vector();
    Box bounds = Bounds();
    if (!bounds.Contains(up))
        return invalid;

    auto& neighbour = (*this)[up];
    auto newAmount = current.Amount() - 1;
    assert(newAmount > 0);
    if (neighbour.IsGas())
    {
        invalid |= up;
        neighbour.Set(current.GetMaterial(), 1);
        assert(current.Amount() > 1);
        invalid |= position;
        current.Increase(-1);
    }
    else if (((neighbour.GetMaterial() == current.GetMaterial()) && (!neighbour.IsSolid()))
        && (neighbour.Amount() < newAmount)
        && (neighbour.Amount() < PackedVoxel::maxAmount))
    {
        invalid |= up;
        neighbour.Increase(1);
        invalid |= position;
        current.Increase(-1);
    }
    else
    {
        return invalid;
    }
    double lerpFactor = 1.0 / neighbour.Amount();  // heat transfered by amount
    double newTemperature = Engine::Lerp(neighbour.Temperature(), current.Temperature(), lerpFactor);
    neighbour.SetTemperature(newTemperature);
    return invalid;
}


Box PackedGrid::Evaporate(double seconds, const Position& position, PackedVoxel& current)
{
    Box invalid;
    // Preconditions, TODO can generalize to all phases and other materials but for now only water vapor is available so 
    if (current.GetMaterial() != &Physics::Material::water)
        return invalid;
    if (!current.IsFluid())
        return invalid;
    auto up = position + Direction::up.Vector();
    Box bounds = Bounds();
    if ((bounds.Contains(up)) && (!(*this)[up].IsGas()))
        return invalid;

    double surface = grid.x*grid.y;
    double evaporationRate = surface * current.GetMaterial()->Evaporation(current.Temperature());
    double mass = current.Density() * grid.Volume();
    double timeToEvaporate = mass / evaporationRate;
    double chance = seconds / timeToEvaporate; // TODO: not exact cummulative chance, but no need to track evopration progress
    if (Engine::Random().Chance() < chance)
    {
        effects.emplace_back(Steam(grid.Center(position), current.Density()*grid.Volume(), current.Temperature()));
        invalid |= position;
        if (auto newAmount = current.Amount() - 1)
        {
            current.Set(current.GetMaterial(), newAmount);
        }
        else
        {
            current.Set(Material::air);
        }
    }
    return invalid;
}

}
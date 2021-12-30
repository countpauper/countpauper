#include "stdafx.h"
#include "DiscreteGrid.h"
#include "Engine/Debug.h"
#include "Engine/Volume.h"
#include "Direction.h"
#include "EnvironmentalEffects.h"

#include <sstream>

namespace Physics
{

DiscreteGrid::DiscreteGrid(const Engine::Vector& extent, const Grid& grid) :
    grid(grid),
    size(int(std::round(extent.x / grid.x)), int(std::round(extent.y / grid.y)), int(std::round(extent.z / grid.z))),
    data(size.Volume())
{
}

Box DiscreteGrid::Bounds() const
{
    return Box(size);
}

Box DiscreteGrid::Neighbourhood(const Position& p) const
{
    return Box(p, Size(1, 1, 1)).Grow(1) & Bounds();
}

unsigned DiscreteGrid::Index(const Position& p) const
{
    return p.x + (p.y * size.x) + (p.z * size.x * size.y);
}



PackedVoxel& DiscreteGrid::operator[](const Position& p)
{
    return data.at(Index(p));
}

const PackedVoxel& DiscreteGrid::operator[](const Position& p) const
{
    return  data.at(Index(p));
}

size_t DiscreteGrid::Fill(const Engine::IVolume& v, Filter filter, const Material& m, double temperature, std::optional<double> density)
{
    size_t filled = 0;
    Box bounds = grid(v.GetBoundingBox()) & Bounds();

    if (!density.has_value())
        density = m.normalDensity;

    double normalDensity = m.normalDensity; //  Density(PascalPerAtmosphere, temperature);
    double fraction = *density / normalDensity;
    for (iterator it(*this, bounds); it != it.end(); ++it)
    {
        // TODO: for water it could be filled with amounts based on vertical levels for even smoother water surface
        auto center = grid.Center(it.position);
        auto& voxel = (*it).second;
        if ((v.Contains(center)) && (filter(center, voxel.GetMaterial(), voxel.Temperature(), voxel.Density())))
        {
            voxel = PackedVoxel(m, temperature, fraction);
            filled++;
        }
    }
    return filled;
}

void DiscreteGrid::Constrain(const Engine::IVolume& v, const Material& m, double temperature, Function density)
{
    constraints.emplace_back(Constraint(v, m, temperature, density));
}

void DiscreteGrid::ApplyForce(const Engine::IVolume& c, const Engine::Vector& v) {}
void DiscreteGrid::ApplyForce(const Engine::Coordinate& c, double force) {}
void DiscreteGrid::Heat(const Engine::Coordinate& c, double energy) {}

is::signals::connection DiscreteGrid::ConnectChange(ChangeSignal::slot_type slot)
{
    return changed.connect(slot);
}

double DiscreteGrid::Density(const Engine::IVolume& c) const
{
    return 0.0;
}

double DiscreteGrid::Temperature(const Engine::IVolume& v) const
{
    Box bounds = grid(v.GetBoundingBox()) & Bounds();
    double count = 0.0;
    double total = 0.0;
    for (const_iterator it(*this, bounds); it != it.end(); ++it)
    {
        if (v.Contains(grid.Center(it.position)))
        {
            count++;
            total += (*it).second.Temperature();
        }
    }
    if (count)
        return total / count;
    else
        return 0;
}

Engine::Vector DiscreteGrid::Force(const Engine::IVolume& c) const
{
    return Engine::Vector();
}

const Material* DiscreteGrid::GetMaterial(const Engine::Coordinate& c) const
{
    auto p = grid(c);
    if (Bounds().Contains(p))
    {
        return (*this)[p].GetMaterial();
    }
    else
    {
        return nullptr;
    }
}

Engine::RGBA DiscreteGrid::Color(const Engine::Line& l) const
{
    Position p = grid(l.b);
    if (Bounds().Contains(p))
    {
        return (*this)[p].Color();
    }
    else
    {   
        return Engine::RGBA();
    }
}

void DiscreteGrid::Tick(double seconds)
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


Box DiscreteGrid::Flow(const Position &position, PackedVoxel& current)
{
    Box invalid;
    invalid |= FlowDown(position, current);
    invalid |= FlowSide(position, current);
    invalid |= FlowUp(position, current);
    return invalid;
}

Box DiscreteGrid::Heat(double seconds, const Position &position, PackedVoxel& current)
{
    Directions halfDirs = Direction::up | Direction::north | Direction::west;
    Box invalid;
    Box bounds = Bounds();
    double t = current.Temperature();
    const auto& material = current.GetMaterial();
    double mass = current.Mass(grid.Volume());
    double energy = mass * t * material->heatCapacity;

    for (auto dir : halfDirs)
    {
        auto dirPosition = position + dir.Vector();
        if (!bounds.Contains(dirPosition))
            continue;
        auto& neighbour = (*this)[dirPosition];
        double nt = neighbour.Temperature();
        if (t == nt)
            continue;

        const auto& neighbour_mat = neighbour.GetMaterial();
        double neighbour_mass = neighbour.Mass(grid.Volume());
        double neighbour_energy = neighbour_mass * nt * neighbour_mat->heatCapacity;

        // use conducivity of coldest, based on nothing except a hunch that it's dominant to spread the energy from the interface
        double conductivity = (t < nt) ? material->conductivity : neighbour_mat->conductivity;
        double rate = dir.Surface(grid) * seconds * conductivity * (t - nt);    // In like ... joules towards the neighbour
        double newTemperature = (energy-rate) / (mass*material->heatCapacity);
        current.SetTemperature(newTemperature);
        double newNeighbourTemperature = (neighbour_energy + rate) / (neighbour_mass * neighbour_mat->heatCapacity);
        neighbour.SetTemperature(newNeighbourTemperature);
        invalid |= position;
        invalid |= dirPosition;
    }

    return invalid;
}


Box DiscreteGrid::FlowDown(const Position &position, PackedVoxel& current)
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

Box DiscreteGrid::FlowSide(const Position &position, PackedVoxel& current)
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

Box DiscreteGrid::FlowUp(const Position &position, PackedVoxel& current)
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


Box DiscreteGrid::Evaporate(double seconds, const Position& position, PackedVoxel& current)
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
    double mass = current.Mass(grid.Volume());
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

std::vector<const Engine::IRendition*> DiscreteGrid::Render() const
{
    std::vector<const Engine::IRendition*> result;
    result.reserve(effects.size());
    for (const auto& effect : effects)
    {
        result.push_back(&effect.GetParticles());
    }
    return result;
}


double DiscreteGrid::Measure(const Material* material, const Engine::IVolume& in) const
{
    double volume = 0.0;
    auto bounds = grid(in.GetBoundingBox()) & Bounds();

    if (material == &Material::vacuum)
    {
        for (const_iterator it(*this, bounds); it != it.end(); ++it)
        {
            if (((*it).second.GetMaterial() == material) && 
                (in.Contains(grid.Center(it.position))))
                volume += grid.Volume() ;
        }
    }
    else
    {
        for (const_iterator it(*this, bounds); it != it.end(); ++it)
        {
            if (((*it).second.GetMaterial() == material) &&
                (in.Contains(grid.Center(it.position))))
                volume += grid.Volume() * (*it).second.Amount() / PackedVoxel::normalAmount;
        }
    }
    return volume;
}

std::wstring DiscreteGrid::Statistics() const
{
    std::wstringstream str;
    str << size.Volume() * sizeof(data[0]) / 1024 << "kB";
    return str.str();
}

}
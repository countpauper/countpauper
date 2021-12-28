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

        if ((current.GetMaterial() == &Material::water) && (current.IsFluid()))
        {
            // first flow down
            auto down = it.position + Direction::down.Vector();
            if (bounds.Contains(down))
            {   // TODO: what if water is bottom, keep?
                auto& neighbour = (*this)[down];
                auto newFill = 0;
                if (neighbour.GetMaterial() == &Material::air)
                {
                    newFill = 1;
                }
                else if (neighbour.GetMaterial() == &Material::water)
                {
                    newFill = neighbour.Amount() + 1;
                }
                if ((newFill > 0) && (newFill < PackedVoxel::normalAmount))
                {

                    auto newAmount = current.Amount() - 1;
                    if (!newAmount)
                    {
                        current.Set(Material::air);
                    }
                    else
                    {
                        current.Set(Material::water, newAmount);
                    }
                    neighbour.Set(Material::water, newFill);
                    invalid |= it.position;
                    invalid |= down;
                    continue;
                }
            }
            // did not continue, flow horizontal, first make a list of options
            // which is neighbours with equal the least amount
            int drainAmount = current.Amount()-1;
            if (drainAmount)
            {   // If the water level >=2 it can flow sideways to lower levels or air. Level 1 just waits to evaporate
                // find flow destinations
                const Directions sides = Direction::north | Direction::south | Direction::east | Direction::west;
                std::vector<Position> flowDestination;
                for (auto dir : sides)
                {
                    auto side = it.position + dir.Vector();
                    if (!bounds.Contains(side))
                        continue;
                    auto& neighbour = (*this)[side];
                    if (neighbour.GetMaterial() == &Material::air)
                    {
                        if (drainAmount> 0)
                            flowDestination.clear();
                        drainAmount = 0;
                        flowDestination.push_back(side);
                    }
                    else if (neighbour.GetMaterial() == &Material::water)
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
                while ((!flowDestination.empty()) && (current.Amount()> drainAmount))
                {
                    // TODO: engine controlled random
                    int idx = rand() % flowDestination.size();

                    auto flowLocation = flowDestination[idx];
                    flowDestination.erase(flowDestination.begin() + idx);
                    invalid |= it.position;
                    invalid |= flowLocation;
                    if (auto newAmount = current.Amount()-1)
                        current.Set(Material::water, newAmount);
                    else
                    {
                        current.Set(Material::air);
                    }
                    auto& neighbour = (*this)[flowLocation];
                    neighbour.Set(Material::water, drainAmount + 1);
                }
            }
            // if water > normal pressure (atmospheric) easily checked with normal amount, potentially flow up 
            if ((current.GetMaterial() == &Material::water) && (current.Amount()>PackedVoxel::normalAmount))
            {
                auto up = it.position + Direction::up.Vector();
                if (bounds.Contains(up))
                {
                    auto& neighbour = (*this)[up];
                    auto newAmount = current.Amount() - 1;
                    assert(newAmount > 0);
                    if (neighbour.IsGas())
                    {
                        invalid |= up;
                        neighbour.Set(Material::water, 1);
                        assert(current.Amount() > 1);
                        invalid |= it.position;
                        current.Increase(-1);
                    }
                    else if ((neighbour.GetMaterial() == current.GetMaterial()) 
                        && (neighbour.Amount() < newAmount)
                        && (neighbour.Amount() < PackedVoxel::maxAmount))
                    {
                        invalid |= up;
                        neighbour.Increase(1);
                        invalid |= it.position;
                        current.Increase(-1);
                    }

                }
            }

            if (current.GetMaterial()==&Material::water)
            {   // evaporate
                double surface = grid.x*grid.y;
                double evaporationRate = current.GetMaterial()->Evaporation(current.Temperature());
                double mass = current.Mass(grid.Volume());
                double timeToEvaporate = mass / evaporationRate;
                double chance = seconds / timeToEvaporate; // TODO: not exact cummulative chance, but no need to track evopration progress
                if (Engine::Random().Chance() < chance)
                {
                    effects.emplace_back(Steam(grid.Center(it.position), current.Density()*grid.Volume(), current.Temperature()));
                    invalid |= it.position;
                    if (auto newAmount = current.Amount() - 1)
                    {
                        current.Set(Material::water, newAmount);
                    }
                    else
                    {
                        current.Set(Material::air);
                    }
                }

            }
        }
    }
    effects.Tick(seconds);
    if (invalid)
        changed(grid.BoundingBox(invalid));
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
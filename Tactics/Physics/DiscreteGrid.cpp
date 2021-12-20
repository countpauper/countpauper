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

    double normalDensity = m.Density(PascalPerAtmosphere, temperature);
    double airDensity = Material::air.Density(PascalPerAtmosphere, temperature);
    double fraction = (*density - airDensity) / (normalDensity - airDensity);
    int amount = int(PackedVoxel::maxAmount * std::min(fraction, 1.0));
    for (iterator it(*this, bounds); it != it.end(); ++it)
    {
        // TODO: for water it could be filled with amounts based on vertical levels for even smoother water surface
        auto center = grid.Center(it.position);
        auto& voxel = (*it).second;
        if ((v.Contains(center)) && (filter(center, voxel.GetMaterial(), voxel.Temperature(), voxel.Density())))
        {
            voxel = PackedVoxel(m, temperature, amount);
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
                if ((newFill > 0) && (newFill < 16))
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
            int amount = current.Amount()-1;       
            if (amount)
            {
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
                        if (amount > 0)
                            flowDestination.clear();
                        amount = 0;
                        flowDestination.push_back(side);
                    }
                    else if (neighbour.GetMaterial() == &Material::water)
                    {
                        if (neighbour.Amount() < amount)
                            flowDestination.clear();
                        if (neighbour.Amount() <= amount)
                        {
                            amount = neighbour.Amount();
                            flowDestination.push_back(side);
                        }
                    }
                }
                // Now spread the current amount over the neighbours
                auto newAmount = current.Amount();
                while ((!flowDestination.empty()) && (newAmount > amount))
                {
                    // TODO: engine controlled random
                    int idx = rand() % flowDestination.size();

                    auto flowLocation = flowDestination[idx];
                    flowDestination.erase(flowDestination.begin() + idx);
                    invalid |= it.position;
                    invalid |= flowLocation;
                    if (--newAmount)
                        current.Set(Material::water, newAmount);
                    else
                        current.Set(Material::air);
                    auto& neighbour = (*this)[flowLocation];
                    neighbour.Set(Material::water, amount + 1);
                }
            }
            else
            {   // water level 1 doesn't flow back, it evaporates (or seeps the ground ?)
                // evaporate (TODO: chance based on temperature from between solid and gas
                effects.emplace_back(Steam(grid.Center(it.position),current.Density()*grid.Volume(), current.Temperature()));
                current.Set(Material::air);
                invalid |= it.position;
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


double DiscreteGrid::Measure(const Material* material) const
{
    double volume = 0.0;
    if (material == &Material::vacuum)
    {
        for (const_iterator it(*this, Bounds()); it != it.end(); ++it)
        {
            if ((*it).second.GetMaterial() == material)
                volume += grid.Volume() ;
        }
    }
    else
    {
        for (const_iterator it(*this, Bounds()); it != it.end(); ++it)
        {
            if ((*it).second.GetMaterial() == material)
                volume += grid.Volume() * (*it).second.Amount() / PackedVoxel::maxAmount;
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
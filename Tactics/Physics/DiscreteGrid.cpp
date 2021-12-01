#include "stdafx.h"
#include "DiscreteGrid.h"
#include "Engine/Debug.h"
#include "Engine/Volume.h"
#include "Direction.h"

#include <sstream>

namespace Physics
{

DiscreteGrid::DiscreteGrid(const Engine::Vector& extent, const Grid& grid) :
    grid(grid),
    size(int(std::round(extent.x / grid.x)), int(std::round(extent.y / grid.y)), int(std::round(extent.z / grid.z))),
    m_data(size.Volume())
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
    return m_data.at(Index(p));
}

const PackedVoxel& DiscreteGrid::operator[](const Position& p) const
{
    return  m_data.at(Index(p));
}

size_t DiscreteGrid::Fill(const Engine::IVolume& v, const Material& m, double temperature)
{
    size_t filled = 0;
    Box bounds = grid(v.GetBoundingBox()) & Bounds();

    for (iterator it(*this, bounds); it != it.end(); ++it)
    {
        if (v.Contains(grid.Center(it.position)))
        {
            (*it).second = PackedVoxel(m, temperature);
            filled++;
        }
    }
    return filled;
}

void DiscreteGrid::ApplyForce(const Engine::IVolume& c, const Engine::Vector& v) {}
void DiscreteGrid::ApplyForce(const Engine::Coordinate& c, double force) {}
void DiscreteGrid::Heat(const Engine::Coordinate& c, double energy) {}

is::signals::connection DiscreteGrid::ConnectChange(ChangeSignal::slot_type slot)
{
    return m_changed.connect(slot);
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
    for (iterator it(*this, bounds); it != it.end(); ++it)
    {
        auto& current = (*it).second;

        if (current.GetMaterial() == &Material::water)
        {
            // TODO: first flow down, only direct neighbours 
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
            int density = current.Amount()-1;            
            Directions sides = Direction::north | Direction::south | Direction::east | Direction::west;
            std::vector<Position> flowDestination;

            for (auto dir : sides)
            {
                auto side = it.position + dir.Vector();
                if (!bounds.Contains(side))
                    continue;
                auto& neighbour = (*this)[side];
                if (neighbour.GetMaterial() == &Material::air)
                {
                    if (density > 0)
                        flowDestination.clear();
                    density = 0;
                    flowDestination.push_back(side);
                }
                else if (neighbour.GetMaterial() == &Material::water)
                {
                    if (neighbour.Amount() < density)
                        flowDestination.clear();
                    if (neighbour.Amount() <= density)
                    {
                        density = neighbour.Amount();
                        flowDestination.push_back(side);
                    }
                }
            }
            auto newAmount = current.Amount();
            while ((!flowDestination.empty()) && (newAmount>density))
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
                neighbour.Set(Material::water, density + 1);
            }
        }
    }
    if (invalid)
        m_changed(grid.BoundingBox(invalid));
}


double DiscreteGrid::Measure(const Material* material) const
{
    double volume = 0.0;
    for (const_iterator it(*this, Bounds()); it != it.end(); ++it)
    {
        if ((*it).second.GetMaterial() == material)
            volume += grid.Volume();
    }
    return volume;
}

std::wstring DiscreteGrid::Statistics() const
{
    std::wstringstream str;
    str << size.Volume() * sizeof(m_data[0]) / 1024 << "kB";
    return str.str();
}

}
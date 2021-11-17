#include "stdafx.h"
#include "DiscreteGrid.h"
#include "Engine/Debug.h"
#include "Engine/Volume.h"

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

void DiscreteGrid::ConnectChange(ChangeSignal::slot_type slot) {}

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
#include "stdafx.h"
#include "DiscreteGrid.h"

namespace Physics
{

DiscreteGrid::DiscreteGrid(const Engine::Vector& extent, const Grid& grid) :
    grid(grid),
    size(int(std::round(extent.x / grid.x)), int(std::round(extent.y / grid.y)), int(std::round(extent.z / grid.z)))
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

is::signals::connection DiscreteGrid::ConnectChange(ChangeSignal::slot_type slot)
{
    return changed.connect(slot);
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

void DiscreteGrid::Constrain(const Engine::IVolume& v, const Material& m, double temperature, Function density)
{
    constraints.emplace_back(Constraint(v, m, temperature, density));
}
}
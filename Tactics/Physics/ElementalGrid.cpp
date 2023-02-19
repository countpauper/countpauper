#include "stdafx.h"
#include "ElementalGrid.h"
#include "Engine/Debug.h"
#include "Physics/Direction.h"

namespace Physics
{

ElementalGrid::ElementalGrid(const Engine::Vector& extent, const Grid& grid) :
    VoxelGrid<ElementalVoxel>(extent, grid),
    m_update([this] { Update(); }, 10.0)
{
}
void ElementalGrid::Tick(double seconds) 
{
    m_update.Tick(seconds);
    time += seconds;
}

void ElementalGrid::Update()
{
    PropagateFire();
    Sun(1);  // ray trace through gas and liquid spreading a certain amount of fire depending on density
    Engine::Debug::Log("Elemental Grid tick @ " + std::to_string(time) + "s");
}

void ElementalGrid::PropagateFire()
{
    Box invalid;
    Box bounds = Bounds();
    // propagate birdirectional in half the directions
    const auto propagationDirections = Directions { Direction::north, Direction::east, Direction::up};

    for (iterator it(*this, bounds); it != it.end(); ++it)
    {
        auto& current = (*it).second;
        for (const auto & dir : propagationDirections)
        {
            auto neightbourPosition = (*it).first + dir.Vector();
            if (bounds.Contains(neightbourPosition))
            {
                auto& neighbour = (*this)[neightbourPosition];
                if (current.PropagateFire(neighbour))
                {
                    // Engine::Debug::Log() << " warmth " << (*it).first << " @ " << current.Temperature() << "K => " << neightbourPosition << " @ " << neighbour.Temperature() << "K";

                    invalid |= (*it).first;
                    invalid |= neightbourPosition;
                }
            }
        }
    }

    changed(grid.BoundingBox(invalid));
}

void ElementalGrid::Sun(int heat)
{
    // TODO: create box data iterator where the direction order can be defined? But break out of z ? weird
    Box invalid;
    Box bounds = Bounds();
    Position p = bounds.Start();
    const Position e = bounds.End();

    for (p.x=bounds.Start().x; p.x < e.x; ++p.x)
    {
        for (p.y = bounds.Start().y; p.y < e.y; ++p.y)
        {
            int verticalHeat = heat;
            for (p.z = e.z-1; p.z>=bounds.Start().z; --p.z)
            {
                if (verticalHeat <= 0)
                    break;
                auto& current = (*this)[p];
                auto attenuation = current.GetMaterial()->opacity * current.Density() * grid.z;
                attenuation = std::min(1.0, attenuation);
                auto dFire = static_cast<int>(std::round(verticalHeat * attenuation));
                if (dFire > 0)
                {
                    current.fire = std::min(static_cast<unsigned>(std::numeric_limits<decltype(current.fire)>::max()), static_cast<unsigned>(current.fire + dFire));
                    //Engine::Debug::Log() << p << " is sun bathing +" << dFire << " = " << current.fire;
                    verticalHeat -= dFire;
                    invalid |= p;
                }
            }
        }
    }
    changed(grid.BoundingBox(invalid));
}

}
#pragma once
#include "Geometry/Position.h"
#include "Game/Material.h"
#include "Geometry/IntBox.h"

namespace Game
{

class HeightMap
{
public:
    virtual Engine::IntBox GetBounds() const = 0;
    virtual float GroundHeight(Engine::Position pos) const = 0;
    Engine::Coordinate GroundCoord(Engine::Position pos) const
    {
        return Engine::Coordinate(
            pos.x + 0.5,
            pos.y + 0.5,
            GroundHeight(pos)
        );
    }
};

}

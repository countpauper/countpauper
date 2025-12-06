#pragma once
#include "Geometry/Position.h"
#include "Game/Material.h"
#include "Game/Position.h"
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
        return Game::Position(pos.x, pos.y, GroundHeight(pos)).Coord();
    }
};

}

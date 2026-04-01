#pragma once
#include "Geometry/Position.h"
#include "Game/Material.h"
#include "Game/Position.h"
#include "Geometry/IntBox.h"

namespace Game
{

class Block;

class HeightMap
{
public:
    virtual Engine::IntBox GetBounds() const = 0;
    virtual float GroundHeight(Position pos) const = 0;
    Engine::Coordinate GroundCoord(Engine::Position pos) const
    {
        return Game::Position(pos.x, pos.y, GroundHeight(Position(pos))).Coord();
    }
    virtual const Block& GetBlock(Engine::Position pos) const = 0;
    const Block& operator[](Engine::Position pos) const  { return GetBlock(pos); }
};

}

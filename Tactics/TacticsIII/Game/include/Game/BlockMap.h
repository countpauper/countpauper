#pragma once
#include "Geometry/Position.h"
#include "Game/Material.h"
#include "Game/Position.h"
#include "Geometry/IntBox.h"
#include "Game/Slice.h"

namespace Game
{

class Block;

class BlockMap
{
public:
    virtual Engine::IntBox GetBounds() const = 0;
    virtual Block GetBlock(Engine::Position pos) const = 0;

    ZType GroundHeight(Position pos) const;
    Engine::Coordinate GroundCoord(Engine::Position pos) const;
    Slice GetSlice(Position pos, ZType height) const;
    Block operator[](Engine::Position pos) const  { return GetBlock(pos); }
};

}

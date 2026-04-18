#pragma once
#include "Geometry/Position.h"
#include "Game/Material.h"
#include "Game/Position.h"
#include "Geometry/IntBox.h"
#include "Game/Slice.h"

namespace Game
{

class MapItf
{
public:
    virtual Engine::IntBox GetBounds() const = 0;
    virtual const Slice& SliceAt(int x, int y) const = 0;
    Slice GetSlice(Position pos, ZType height) const;

    ZType GroundHeight(Position pos) const;
    Engine::Coordinate GroundCoord(Engine::Position pos) const;
};

}

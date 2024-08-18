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
    virtual Engine::Coordinate GroundCoord(Engine::Position pos) const = 0;
    virtual int GroundHeight(Engine::Position pos) const = 0;
    virtual const Material& GetMaterial(Engine::Position pos) const = 0;
};

}

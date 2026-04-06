#pragma once
#include "Geometry/Position.h"

namespace Game
{
class Actor;
class BlockMap;

class World
{
public:
    virtual const BlockMap& GetMap() const = 0;
    virtual bool Obstacle(Engine::Position at, const Actor* except=nullptr) const =0;
};

}

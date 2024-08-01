#pragma once
#include "Geometry/Position.h"

namespace Game
{
class Actor;
class HeightMap;

class World
{
public:
    virtual const HeightMap& GetMap() const = 0;
    virtual bool Obstacle(Engine::Position at, const Actor* except=nullptr) const =0;
};

}

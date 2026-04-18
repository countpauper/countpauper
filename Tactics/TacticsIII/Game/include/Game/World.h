#pragma once
#include "Geometry/Position.h"

namespace Game
{
class Actor;
class MapItf;

class World
{
public:
    virtual const MapItf& GetMap() const = 0;
    virtual bool Obstacle(Position at, const Actor* except=nullptr) const =0;
};

}

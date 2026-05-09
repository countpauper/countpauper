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
    virtual const Actor* ObstacleAt(const Engine::IntBox& bounds, const Actor* except=nullptr) const =0;
};


}

#pragma once

#include "Physics/Position.h"

namespace Game
{

struct Position : public Physics::Position
{
public:
    Position()=default;
    Position(int x, int y, int z) : Physics::Position(x, y, z) {}
    Position(const Physics::Position& o) : Position(o.x, o.y, o.z) {}
    unsigned DistanceEl(const Position& other) const;
};

}

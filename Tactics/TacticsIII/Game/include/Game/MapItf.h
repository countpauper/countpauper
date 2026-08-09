#pragma once
#include "Game/Position.h"
#include "Geometry/IntBox.h"

namespace Game
{

class Stack;

class MapItf
{
public:
    virtual Engine::IntBox GetBounds() const = 0;
    virtual const Stack& StackAt(int x, int y) const = 0;
    inline const Stack& operator[](int x, int y) const { return StackAt(x,y); };
    Stack GetStack(Position pos, ZType height) const;

    ZType GroundHeight(Position pos) const;
    Engine::Range<ZType> Z() const;
    Engine::Coordinate GroundCoord(Engine::Position pos) const;
};

}

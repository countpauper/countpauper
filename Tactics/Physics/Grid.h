#pragma once
#include "Engine/Vector.h"
#include "Engine/Coordinate.h"
#include "Physics/Position.h"

namespace Physics
{

class Grid : public Engine::Vector
{
public:
    Grid(const Vector& v) : Vector(v) {}

    Engine::Coordinate Center(const Physics::Position& p) const
    {
        return Engine::Coordinate((double(p.x) + 0.5)*x, (double(p.y) + 0.5)*y, (double(p.z) + 0.5)*z);
    }
    Position operator()(const Engine::Coordinate& c) const
    {
        return Position(int(std::floor(c.x / x)), int(std::floor(c.y / y)), int(std::floor(c.z / z)));
    }
    double Volume() const
    {
        return x * y * z;
    }
};

} // ::Physics
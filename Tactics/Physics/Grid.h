#pragma once
#include "Engine/Vector.h"
#include "Engine/Coordinate.h"
#include "Physics/Position.h"
#include "Physics/Box.h"
#include "Engine/AxisAlignedBoundingBox.h"

namespace Physics
{

class Grid : public Engine::Vector
{
public:
    Grid() : Vector(1.0, 1.0, 1.0) {}
    Grid(double x, double y, double z) : Vector(x, y, z) {}
    Grid(const Vector& v) : Vector(v) {}

    operator Vector()
    {
        return Vector(x, y, z);
    }

    Engine::Coordinate Center(const Physics::Position& p) const;
    Engine::Vector Meters(const Size& size) const;
    Position operator()(const Engine::Coordinate& c) const;
    Box operator()(const Engine::AABB& b) const;
    double Volume() const;
};

} // ::Physics
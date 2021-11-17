#include "stdafx.h"
#include "Physics/Grid.h"
#include "Engine/Matrix.h"

namespace Physics
{

Engine::Coordinate Grid::Center(const Physics::Position& p) const
{
    return Engine::Coordinate((double(p.x) + 0.5)*x, (double(p.y) + 0.5)*y, (double(p.z) + 0.5)*z);
}

Engine::Vector Grid::Meters(const Size& size) const
{
    return Engine::Vector(size.x * x, size.y * y, size.z * z);
}
Position Grid::operator()(const Engine::Coordinate& c) const
{
    return Position(int(std::floor(c.x / x)), int(std::floor(c.y / y)), int(std::floor(c.z / z)));
}

Box Grid::operator()(const Engine::AABB& b) const
{
    return Box(b.x.finite() ? Engine::Range<int>(int(std::floor(b.x.begin / x)), int(std::ceil(b.x.end / x ))) : Engine::Range<int>::max(),
        b.y.finite() ? Engine::Range<int>(int(std::floor(b.y.begin / y)), int(std::ceil(b.y.end / y ))) : Engine::Range<int>::max(),
        b.z.finite() ? Engine::Range<int>(int(std::floor(b.z.begin / z)), int(std::ceil(b.z.end / z ))) : Engine::Range<int>::max());
}

double Grid::Volume() const
{
    return x * y * z;
}

} // ::Physics
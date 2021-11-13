#include "stdafx.h"
#include "Physics/Grid.h"
#include "Engine/Matrix.h"

namespace Physics
{

Engine::Coordinate Grid::Center(const Physics::Position& p) const
{
    return Engine::Coordinate((double(p.x) + 0.5)*x, (double(p.y) + 0.5)*y, (double(p.z) + 0.5)*z);
}
Position Grid::operator()(const Engine::Coordinate& c) const
{
    return Position(int(std::floor(c.x / x)), int(std::floor(c.y / y)), int(std::floor(c.z / z)));
}

Box Grid::operator()(const Engine::AABB& b) const
{
    Engine::AABB sb = b * Engine::Matrix::Scale(*this);

    return Box(Engine::Range<int>(int(std::floor(sb.x.begin)), int(std::floor(sb.x.end))),
        Engine::Range<int>(int(std::floor(sb.y.begin)), int(std::floor(sb.y.end))),
        Engine::Range<int>(int(std::floor(sb.z.begin)), int(std::floor(sb.z.end))));
}

double Grid::Volume() const
{
    return x * y * z;
}

} // ::Physics
#include <sstream>
#include "Utility/Maths.h"
#include "Game/Position.h"
#include "Geometry/Coordinate.h"

namespace Game
{

Position::Position() :
    x(0), 
    y(0),
    z(0)
{
}

Position::Position(int x, int y, Position::ZType z) :
    x(x),
    y(y),
    z(z)
{
}

Position::Position(const Engine::Position& p, float zo) :
    x(p.x),
    y(p.y),
    z(p.z + zo)
{

}

Position Position::ProjectHorizontal() const 
{ 
    return Position(X(), Y(), 0);
}

float Position::ManDistance(Position other) const
{
    return std::abs(other.X() - X()) +
        std::abs(other.Y() - Y()) +
        static_cast<float>(abs(other.Z() - Z()));
}

float Position::Length() const
{
    return std::sqrt(Engine::Sqr(float(X())) + Engine::Sqr(float(Y())) + Engine::Sqr(static_cast<float>(Z())));
}

float Position::Distance(Position other) const
{
    return std::sqrt(Engine::Sqr(float(other.X() - X())) +
            Engine::Sqr(float(other.Y() - Y())) +
            Engine::Sqr(float(other.Z() - Z())));
}

Position& Position::operator+=(Position delta)
{
    x+= delta.x;
    y+= delta.y;
    z+= delta.z;
    return *this;
}
Position& Position::operator-=(Position delta)
{
    x -= delta.x;
    y -= delta.y;
    z -= delta.z;
    return *this;
}

Engine::Coordinate Position::Coord() const
{
    return Engine::Coordinate{
        static_cast<double>(x)+0.5,
        static_cast<double>(y)+0.5,
        static_cast<double>(Z())
    };
}
Position::operator bool() const
{
    return X() != 0 || Y() != 0 || Z() != 0.0f;
}

Position operator+(Position a, Position b)
{
    Position o(a);
    o += b;
    return o;
}
Position operator-(Position a, Position b)
{
    Position o(a);
    o -= b;
    return o;
}

std::ostream& operator<<(std::ostream& stream, Position position)
{
    stream << "(" << position.X() << ", " << position.Y() << ", " << position.Z() << ")";
    return stream;
}

Engine::Position round(Position p)
{
     return Engine::Position(p.x, p.y, round(p.z)); 
}

bool operator==(Position a, Position b)
{
    return a.X() == b.X() && 
        a.Y() == b.Y() && 
        a.Z() == b.Z();
}
bool operator!=(Position a, Position b)
{
    return !operator==(a, b);
}

bool operator<(Position a, Position b)
{
    if (a.Z() < b.Z())
        return true;
    else if (a.Z() == b.Z())
    {
        if (a.Y() < b.Y())
            return true;
        else if (a.Y() == b.Y())
        {
            return a.X() < b.X();
        }
    }
    return false;
}

}


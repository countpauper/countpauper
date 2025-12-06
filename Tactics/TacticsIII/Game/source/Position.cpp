#include <sstream>
#include "Utility/Maths.h"
#include "Game/Position.h"
#include "Geometry/Coordinate.h"

namespace Game
{

Position::Position() :
    p(),
    z_offset(0.0)
{
}

Position::Position(int x, int y, double z) :
    p(x, y, (int)z),
    z_offset(z - p.z)
{
}

double Position::Z() const
{
    return static_cast<double>(p.z) + z_offset;
}


double Position::ManDistance(Position other) const
{
    return std::abs(other.p.x - p.x) +
        std::abs(other.p.y - p.y) +
        std::abs(other.Z() - Z());
}

double Position::Size() const
{
    return std::sqrt(Engine::Sqr(double(p.x)) + Engine::Sqr(double(p.y)) + Engine::Sqr(Z()));
}

double Position::Distance(Position other) const
{
    return std::sqrt(Engine::Sqr(double(other.p.x - p.x)) +
            Engine::Sqr(double(other.p.y - p.y)) +
            Engine::Sqr(double(other.p.z -p.z) + (other.z_offset - z_offset)));
}

Position& Position::operator+=(Position delta)
{
    p += delta.p;
    z_offset += delta.z_offset;
    int z_overflow = std::floor(z_offset);
    p.z += z_overflow;
    z_offset -= z_overflow;
    return *this;
}
Position& Position::operator-=(Position delta)
{
    p -= delta.p;
    z_offset -= delta.z_offset;
    int z_underflow = std::floor(z_offset);
    p.z += z_underflow;
    z_offset -= z_underflow;
    return *this;
}

Engine::Coordinate Position::Coord() const
{
    return Engine::Coordinate{
        static_cast<double>(p.x)+0.5,
        static_cast<double>(p.y)+0.5,
        Z()
    };
}
Position::operator bool() const
{
    return bool(p) || z_offset!=0.0f;
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
    stream << "(" << position.p.x << ", " << position.p.y << ", " << position.Z() << ")";
    return stream;
}
std::wostream& operator<<(std::wostream& stream, Position position)
{
    stream << L"(" << position.p.x << L", " << position.p.y << L", " << position.Z() << L")";
    return stream;
}
bool operator==(Position a, Position b)
{
    return a.p == b.p && a.z_offset == b.z_offset;
}
bool operator!=(Position a, Position b)
{
    return !operator==(a, b);
}

bool operator<(Position a, Position b)
{
    if (a.Z() > b.Z())
        return false;
    else if (a.Z() == b.Z())
    {
        if (a.p.y > b.p.y)
            return false;
        else if (a.p.y == b.p.y)
        {
            return a.p.x < b.p.x;
        }
    }
    return false;
}

}

#include <sstream>
#include "Utility/Maths.h"
#include "Geometry/Position.h"

namespace Engine
{

    Position::Position() : x(0), y(0), z(0)
    {
    }

    Position::Position(int x, int y, int z) : x(x), y(y), z(z)
    {
    }

    Position::Position(Axis axis) :
        Position()
    {
        switch(axis)
        {
            case Axis::NegZ:
                z = -1;
                break;
            case Axis::NegY:
                y = -1;
                break;
            case Axis::NegX:
                x = -1;
                break;
            case Axis::PosX:
                x = 1;
                break;
            case Axis::PosY:
                y = 1;
                break;
            case Axis::PosZ:
                z = 1;
                break;
        }
    }

    unsigned Position::ManDistance(Position other) const
    {
        return std::abs(other.x - x) +
            std::abs(other.y - y) +
            std::abs(other.z - z);
    }

    float Position::Size() const
    {
        return std::sqrt(Engine::Sqr(float(x)) + Engine::Sqr(float(y))+Engine::Sqr(float(z)));
    }

    float Position::Distance(Position other) const
    {
        return std::sqrt(Engine::Sqr(float(other.x - x)) + Engine::Sqr(float(other.y - y)) + Engine::Sqr(float(other.z -z)));
    }

    Position& Position::operator+=(Position delta)
    {
        x += delta.x;
        y += delta.y;
        z += delta.z;
        return *this;
    }
    Position& Position::operator-=(Position delta)
    {
        x -= delta.x;
        y -= delta.y;
        z -= delta.z;
        return *this;
    }
    Position::operator bool() const
    {
        return x != 0 || y != 0 || z != 0;
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
        stream << "(" << position.x << ", " << position.y << ", " << position.z << ")";
        return stream;
    }
    std::wostream& operator<<(std::wostream& stream, Position position)
    {
        stream << L"(" << position.x << L", " << position.y << L", " << position.z << L")";
        return stream;
    }
    bool operator==(Position a, Position b)
    {
        return a.x == b.x && a.y == b.y && a.z == b.z;
    }
    bool operator!=(Position a, Position b)
    {
        return !operator==(a, b);
    }

    bool operator<(Position a, Position b)
    {
        if (a.z > b.z)
            return false;
        else if (a.z == b.z)
        {
            if (a.y > b.y)
                return false;
            else if (a.y == b.y)
            {
                return a.x < b.x;
            }
        }
        return false;
    }
}

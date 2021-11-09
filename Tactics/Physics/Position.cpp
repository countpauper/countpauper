#include "stdafx.h"
#include <sstream>
#include "Engine/Maths.h"
#include "Physics/Position.h"

namespace Physics
{

    Position::Position() : x(0), y(0), z(0)
    {
    }
    Position::Position(int x, int y, int z) : x(x), y(y), z(z)
    {
    }

    unsigned Position::ManDistance(const Position& other) const
    {
        return std::abs(other.x - x) +
            std::abs(other.y - y) +
            std::abs(other.z - z);
    }

    float Position::Size() const
    {
        return std::sqrtf(Engine::Sqr(float(x)) + Engine::Sqr(float(y))+Engine::Sqr(float(z)));
    }

    float Position::Distance(const Position& other) const
    {
        return std::sqrtf(Engine::Sqr(float(other.x - x)) + Engine::Sqr(float(other.y - y)) + Engine::Sqr(float(other.z -z)));
    }

    Position& Position::operator+=(const Position& delta)
    {
        x += delta.x;
        y += delta.y;
        z += delta.z;
        return *this;
    }
    Position& Position::operator-=(const Position& delta)
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

    Position operator+(const Position& a, const Position& b)
    {
        Position o(a);
        o += b;
        return o;
    }
    Position operator-(const Position& a, const Position& b)
    {
        Position o(a);
        o -= b;
        return o;
    }

    std::ostream& operator<<(std::ostream& stream, const Position& position)
    {
        stream << "(" << position.x << ", " << position.y << ", " << position.z << ")";
        return stream;
    }
    std::wostream& operator<<(std::wostream& stream, const Position& position)
    {
        stream << L"(" << position.x << L", " << position.y << L", " << position.z << L")";
        return stream;
    }
    bool operator==(const Position& a, const Position& b)
    {
        return a.x == b.x && a.y == b.y && a.z == b.z;
    }
    bool operator!=(const Position& a, const Position& b)
    {
        return !operator==(a, b);
    }

    bool operator<(const Position& a, const Position& b)
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

    Size operator+(const Size& a, const Size& b)
    {
        Size o = a;
        o.x += b.x;
        o.y += b.y;
        o.z += b.z;
        return o;
    }

    Size operator+(const Size& a, int s)
    {
        return a + Size(s, s, s);
    }
} // ::Game
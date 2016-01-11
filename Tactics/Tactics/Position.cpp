#include "stdafx.h"
#include "Position.h"

namespace Game
{

    Position::Position() : x(0), y(0) 
    {
    }
    Position::Position(int x, int y) : x(x), y(y) 
    {
    }

    unsigned Position::ManDistance(const Position& other) const
    {
        return std::abs(other.x - x) +
            std::abs(other.y - y);
    }

    float Position::Distance(const Position& other) const
    {
        return std::sqrtf(sqr(float(other.x - x)) + sqr(float(other.y - y)));
    }

    Position& Position::operator+=(const Position& delta)
    {
        x += delta.x;
        y += delta.y;
        return *this;
    }
    Position& Position::operator-=(const Position& delta)
    {
        x -= delta.x;
        y -= delta.y;
        return *this;
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

    bool operator==(const Position& a, const Position& b)
    {
        return a.x == b.x && a.y == b.y;
    }
    bool operator!=(const Position& a, const Position& b)
    {
        return !operator==(a, b);
    }

} // ::Game
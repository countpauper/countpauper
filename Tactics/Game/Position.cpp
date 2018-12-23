#include "stdafx.h"
#include <sstream>
#include "Engine/Geometry.h"
#include "Game/Position.h"

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

    unsigned Position::ManSize() const
    {
        return std::abs(x) + std::abs(y);
    }

    float Position::Size() const
    {
        return std::sqrtf(Engine::sqr(float(x)) + Engine::sqr(float(y)));
    }


    float Position::Distance(const Position& other) const
    {
        return std::sqrtf(Engine::sqr(float(other.x - x)) + Engine::sqr(float(other.y - y)));
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
    Position::operator bool() const
    {
        return x != 0 || y!= 0;
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

    std::wstring Position::Description() const
    {
        std::wstringstream ss;
        ss << x << L", " << y;
        return ss.str();
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
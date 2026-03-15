#pragma once
#include <iostream>

namespace Engine
{
    struct Position
    {
        constexpr Position() : x(0), y(0), z(0) { }
        constexpr Position(int x, int y, int z=0) : x(x), y(y), z(z) { }

        int x;
        int y;
        int z;

        unsigned ManDistance(Position other) const;
        float Distance(Position other) const;
        float Size() const;
        Position& operator+=(Position delta);
        Position& operator-=(Position delta);
        explicit constexpr operator bool() const { return  x != 0 || y != 0 || z != 0; }
    };

	bool operator==(Position a, Position b);
    bool operator!=(Position a, Position b);
    bool operator<(Position a, Position b); // for map keys
    Position operator+(Position a, Position b);
    Position operator-(Position a, Position b);
    std::ostream& operator<<(std::ostream& stream, Position position);
    std::wostream& operator<<(std::wostream& stream, Position position);
}   // ::Engine


template<>
struct std::hash<Engine::Position>
{
    size_t operator()(const Engine::Position &p) const
    {
        return p.x + p.y*256 + p.z*65536;
    }
};

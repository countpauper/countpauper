#pragma once
#include <iostream>


namespace Engine
{
    struct Position
    {
        Position();
        Position(int x, int y, int z=0);
        int x;
        int y;
        int z;
        unsigned ManDistance(Position other) const;
        float Distance(Position other) const;
        float Size() const;
        Position& operator+=(Position delta);
        Position& operator-=(Position delta);
        explicit operator bool() const;
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

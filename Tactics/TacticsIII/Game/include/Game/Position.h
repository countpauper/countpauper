#pragma once
#include "Geometry/Position.h"
#include "Geometry/Coordinate.h"
#include <cmath>

namespace Game
{
    using Fraction=float;   // TODO fixed point

    struct Position
    {
        Position();
        Position(int x, int y, float z=0.0);
        explicit Position(const Engine::Position& p, float zo=0.0);

        int x,y;
        float z;

        inline int X() const { return x; }
        inline int Y() const { return y; }
        inline float Z() const { return z; }

        Position ProjectHorizontal() const;
        float ManDistance(Position other) const;
        float Distance(Position other) const;
        float Length() const;
        Engine::Coordinate Coord() const;

        Position& operator+=(Position delta);
        Position& operator-=(Position delta);
        explicit operator bool() const;
    };

    using Size = Position;

    Engine::Position round(Position p);

	bool operator==(Position a, Position b);
    bool operator!=(Position a, Position b);
    bool operator<(Position a, Position b); // for map keys
    Position operator+(Position a, Position b);
    Position operator-(Position a, Position b);

    std::ostream& operator<<(std::ostream& stream, Position position);

}   // ::Game


template<>
struct std::hash<Game::Position>
{
    size_t operator()(const Game::Position &p) const
    {
        return p.X() + p.Y()*256 + static_cast<size_t>(std::round(p.Z()*1024)*65536);
    }
};
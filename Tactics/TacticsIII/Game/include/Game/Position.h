#pragma once
#include "Geometry/Position.h"
#include "Geometry/Coordinate.h"

namespace Game
{
    using Fraction=float;   // TODO fixed point

    struct Position
    {
        Position();
        Position(int x, int y, float z=0.0);
        explicit Position(const Engine::Position& p, float zo=0.0);

        Engine::Position p;
        Fraction z_offset;

        inline int X() const { return p.x; }
        inline int Y() const { return p.y; }
        inline float Z() const { return  static_cast<float>(p.z) + z_offset; }

        Position ProjectHorizontal() const;
        double ManDistance(Position other) const;
        double Distance(Position other) const;
        double Length() const;
        Engine::Coordinate Coord() const;

        Position& operator+=(Position delta);
        Position& operator-=(Position delta);
        explicit operator bool() const;
    };

    using Size = Position;

    Position round(Position p);

	bool operator==(Position a, Position b);
    bool operator!=(Position a, Position b);
    bool operator<(Position a, Position b); // for map keys
    Position operator+(Position a, Position b);
    Position operator-(Position a, Position b);

}   // ::Game

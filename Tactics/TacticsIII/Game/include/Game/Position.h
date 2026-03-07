#pragma once
#include "Geometry/Position.h"
#include "Geometry/Coordinate.h"

namespace Game
{
    using Fraction=double;   // TODO fixed point

    struct Position
    {
        Position();
        Position(int x, int y, double z=0.0);
        Engine::Position p;
        Fraction z_offset;

        inline int X() const { return p.x; }
        inline int Y() const { return p.y; }
        inline double Z() const { return  static_cast<double>(p.z) + z_offset; }
        double ManDistance(Position other) const;
        double Distance(Position other) const;
        double Length() const;
        Engine::Coordinate Coord() const;

        Position& operator+=(Position delta);
        Position& operator-=(Position delta);
        explicit operator bool() const;
    };

    using Size = Position;

	bool operator==(Position a, Position b);
    bool operator!=(Position a, Position b);
    bool operator<(Position a, Position b); // for map keys
    Position operator+(Position a, Position b);
    Position operator-(Position a, Position b);

}   // ::Game

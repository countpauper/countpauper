#pragma once
#include "Geometry/Position.h"

namespace Game
{
    using Fraction=double;   // TODO fixed point

    struct Position
    {
        Position();
        Position(int x, int y, double z=0.0);
        Engine::Position p;
        Fraction z_offset;

        double ManDistance(Position other) const;
        double Distance(Position other) const;
        double Size() const;
        Position& operator+=(Position delta);
        Position& operator-=(Position delta);
        explicit operator bool() const;
    };

	bool operator==(Position a, Position b);
    bool operator!=(Position a, Position b);
    bool operator<(Position a, Position b); // for map keys
    Position operator+(Position a, Position b);
    Position operator-(Position a, Position b);

}   // ::Game

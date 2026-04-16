#pragma once
#include "Geometry/Position.h"
#include "Geometry/Coordinate.h"
#include "Utility/FixedPoint.h"
#include <cmath>

namespace Game
{
    using ZType = Engine::FixedPoint<8>;   

    struct Position
    {
        Position();
        using ZType = Game::ZType;
        Position(int x, int y, ZType z=ZType(0));
        explicit Position(const Engine::Position& p, float zo=0.0);



        inline int X() const { return x; }
        inline int Y() const { return y; }
        inline ZType Z() const { return z; }

        Position ProjectHorizontal() const;
        float ManDistance(Position other) const;
        float Distance(Position other) const;
        float Length() const;
        Engine::Coordinate Coord() const;

        Position& operator+=(Position delta);
        Position& operator-=(Position delta);
        explicit operator bool() const;

        int x,y;
        ZType z;
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
        return p.X() + p.Y()*256 + static_cast<size_t>(p.Z().RawValue())*65536; // TODO: std::hash for FixedPoint and use that instead of RawValue, make it private 
    }
};
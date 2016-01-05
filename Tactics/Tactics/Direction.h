#pragma once

#include <map>
#include "Position.h"

namespace Game
{
    class Direction
    {
    public:
        enum class Value
        {
            North = 1,
            East = 2,
            South = 4,
            West = 8,
        };
        Direction();
        Direction(Value direction);
        Position Vector() const;
        float Angle() const;
    protected:
        Value value;
        static std::map<Value, Position> vector;
        static std::map<Value, float> angle;
    };

	float Rad2Deg(float rad); 
    float ShortestTurn(float a, float b);
}    // ::Game

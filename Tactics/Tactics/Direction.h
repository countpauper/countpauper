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
        Direction(const Position& vector);
        Position Vector() const;
        float Angle() const;
        std::wstring Description() const;
    protected:
        Value value;
        static std::map<Value, Position> vector;
        static std::map<Value, float> angle;
        static std::map<Value, std::wstring> description;
    };

    float Rad2Deg(float rad); 
    float ShortestTurn(float a, float b);
}    // ::Game

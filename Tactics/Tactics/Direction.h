#pragma once

#include <map>
#include "Position.h"

namespace Game
{
    class Direction
    {
    public:
        enum Value
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

    enum class Plane
    {
        None = 0,
        Left = 1 << 8,
        Right = 1 << 9,
        Coronal = Left + Right, 
        Front = 1 << 10,  
        Back = 1 << 11,
        Sagittal = Front + Back,
        Top = 1 << 12,
        Bottom = 1 << 13,
        Transversal = Top + Bottom,
        Around = Coronal + Sagittal,
        All = Around + Transversal
    };
    std::wistream& operator>>(std::wistream& s, Plane& plane);

    class AttackVector
    {
    public:
        Plane plane;
        unsigned height;
        bool Match(const AttackVector other) const;
    };
    std::wistream& operator>>(std::wistream& s, AttackVector& v);

}    // ::Game

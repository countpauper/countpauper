#pragma once

#include <map>
#include "Position.h"

namespace Game
{

// TODO: rename orientation
class Direction
{
public:
    enum Value
    {
        None = 0,
        North = 1,
        East = 2,
        South = 4,
        West = 8,
        Horizontal = North | East | South | West,
        Down = 16,
        Up = 32,
        Vertical = Down|Up,
    };
    Direction();
    Direction(Value direction);
    explicit Direction(const Position& vector);
    Position Vector() const;
    double Angle() const;
    bool Opposite(const Direction& other) const;
    bool Clockwise(const Direction& other) const;
    bool CounterClockwise(const Direction& other) const;
    bool Prone() const;
    void Fall();

    std::wstring Description() const;

    bool IsNone() const;    // operator bool leads to implicit conversion confusion with operator==
    bool operator==(const Direction& other) const;
    bool operator<(const Direction& other) const;
protected:
    Value value;
    Value HorizontalDirection() const;
    static Value From(const Position& vector);
    static std::map<Value, Position> vector;
    static std::map<Value, float> angle;
    static std::map<Value, std::wstring> description;
};

static inline std::ostream& operator<< (std::ostream& os, const Direction& dir)
{
    os << dir.Description().c_str();
    return os;
}

}    // ::Game

#include "stdafx.h"
#include "Game/Direction.h"
#include "Engine/Geometry.h"

namespace Game
{
Direction::Direction() :
    value(Direction::Value::North)
{

}
Direction::Direction(Direction::Value value) :
    value(value)
{
}

Direction::Value Direction::From(const Position& vector)
{
    auto x = vector.x;
    auto y = vector.y;
    if (x>y && x<=-y)
        return Direction::North;
    if (x >= y && x > -y)
        return Direction::East;
    if (x<y && x>= -y)
        return Direction::South;
    if (x <= y && x < -y)
        return Direction::West;
    assert(x==0 && y==0);
    return Direction::None;
}

Direction::Direction(const Position& desiredVector) :
    value(From(desiredVector))
{
}
    
Position Direction::Vector() const
{
    return vector.at(value);
}
double Direction::Angle() const
{
    return angle.at(value);
}

bool Direction::Opposite(const Direction& other) const
{
    switch (value)
    {
    case North:
        return other.value == South;
    case East:
        return other.value == West;
    case South:
        return other.value == North;
    case West:
        return other.value == East;
    default:
        return false;
    }
}
bool Direction::Clockwise(const Direction& other) const
{
    switch (value)
    {
    case North:
        return other.value == West;
    case East:
        return other.value == North;
    case South:
        return other.value == East;
    case West:
        return other.value == South;
    default:
        return false;
    }
}

bool Direction::CounterClockwise(const Direction& other) const
{
    switch (value)
    {
    case North:
        return other.value == East;
    case East:
        return other.value == South;
    case South:
        return other.value == West;
    case West:
        return other.value == North;
    default:
        return false;
    }
}

std::wstring Direction::Description() const
{
    return description.at(value);
}

bool Direction::IsNone() const
{
    return value == Direction::None;
}

bool Direction::operator==(const Direction& other) const
{
    return value == other.value;
}
bool Direction::operator<(const Direction& other) const
{
    return value < other.value;
}

std::map<Direction::Value, Position> Direction::vector =
{
    { Direction::Value::None, Position(0, 0) },
    { Direction::Value::North, Position(0, -1) },
    { Direction::Value::East, Position(1, 0) },
    { Direction::Value::South, Position(0, 1) },
    { Direction::Value::West, Position(-1, 0) },
};
std::map<Direction::Value, float> Direction::angle =
{
    { Direction::Value::None, std::numeric_limits<float>::quiet_NaN() },
    { Direction::Value::North, float(Engine::PI) * 0.5f },
    { Direction::Value::East, 0.0f },
    { Direction::Value::South, float(Engine::PI) * -.5f },
    { Direction::Value::West, float(Engine::PI) },
};

std::map<Direction::Value, std::wstring> Direction::description =
{
    { Direction::Value::None, L"" },
    { Direction::Value::North, L"North" },
    { Direction::Value::East, L"East" },
    { Direction::Value::South, L"South" } ,
    { Direction::Value::West, L"West" },
};



}    // ::Game

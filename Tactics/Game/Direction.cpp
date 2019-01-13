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
    return vector.at(HorizontalDirection());
}
double Direction::Angle() const
{
    return angle.at(HorizontalDirection());
}

Direction::Value Direction::HorizontalDirection() const
{
    return Value(unsigned(value)&Horizontal);
}

bool Direction::Opposite(const Direction& other) const
{
    switch (HorizontalDirection())
    {
    case North:
        return other.HorizontalDirection() == South;
    case East:
        return other.HorizontalDirection() == West;
    case South:
        return other.HorizontalDirection() == North;
    case West:
        return other.HorizontalDirection() == East;
    default:
        return false;
    }
}
bool Direction::Clockwise(const Direction& other) const
{
    switch (HorizontalDirection())
    {
    case North:
        return other.HorizontalDirection() == West;
    case East:
        return other.HorizontalDirection() == North;
    case South:
        return other.HorizontalDirection() == East;
    case West:
        return other.HorizontalDirection() == South;
    default:
        return false;
    }
}

bool Direction::CounterClockwise(const Direction& other) const
{
    switch (HorizontalDirection())
    {
    case North:
        return other.HorizontalDirection() == East;
    case East:
        return other.HorizontalDirection() == South;
    case South:
        return other.HorizontalDirection() == West;
    case West:
        return other.HorizontalDirection() == North;
    default:
        return false;
    }
}

bool Direction::Prone() const
{
    return (value & Down) || (value&Up);
}

void Direction::Fall()
{
    value = Value(unsigned(value) | Down);
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

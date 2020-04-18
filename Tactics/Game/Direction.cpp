#include "stdafx.h"
#include "Game/Direction.h"
#include "Engine/Geometry.h"

namespace Game
{



HalfPiAngle::HalfPiAngle() = default;

HalfPiAngle::HalfPiAngle(int value) : value(value)
{
}

bool HalfPiAngle::operator==(const HalfPiAngle& other) const
{
	return value == other.value;
}

HalfPiAngle& HalfPiAngle::operator+=(const HalfPiAngle& other)
{
	value += other.value;
	return *this;
}

HalfPiAngle operator+(const HalfPiAngle& a, const HalfPiAngle& b)
{
	HalfPiAngle o(a);
	o += b;
	return o;
}

HalfPiAngle& HalfPiAngle::operator-=(const HalfPiAngle& other)
{
	value -= other.value;
	return *this;
}
HalfPiAngle operator-(const HalfPiAngle& a, const HalfPiAngle& b)
{
	HalfPiAngle o(a);
	o -= b;
	return o;
}


double HalfPiAngle::Angle() const
{
	return static_cast<double>(value) * 0.5 * Engine::PI;
}

HalfPiAngle& HalfPiAngle::Normalize()
{
	value %= 4;
	if (value > 2)
		value -= 4;
	if (value < -1)
		value += 4;
	return *this;
}

Direction::Direction() :
	value(None)
{
}

Direction::Direction(Direction::Value value) :
	value(value)
{
}

Direction::Direction(uint8_t id) :
    Direction(static_cast<Value>(id))
{
}

uint8_t Direction::Id() const
{
    return static_cast<uint8_t>(value);
}

const Direction Direction::none;
const Direction Direction::east(Direction::East);
const Direction Direction::north(Direction::North);
const Direction Direction::west(Direction::West);
const Direction Direction::south(Direction::South);
const Direction Direction::up(Direction::Up);
const Direction Direction::down(Direction::Down);
const Direction& Direction::forward = east;
const Direction& Direction::left = north;
const Direction& Direction::backward = west;
const Direction& Direction::right = south;
const std::map<const std::wstring, Direction> Direction::map(
{
	{ L"east", Direction::east },
	{ L"north", Direction::north },
	{ L"west", Direction::west },
	{ L"south", Direction::south },
	{ L"forward", Direction::forward },
	{ L"backward", Direction::backward },
	{ L"right", Direction::right },
	{ L"left", Direction::left },
	{ L"up",Direction::up },
	{ L"down", Direction::down },
});

Direction::Value Direction::From(const Position& vector, int z)
{
	auto x = vector.x;
	auto y = vector.y;
	if ((z > std::abs(x)) && (z > std::abs(y)))
	{
		return Up;
	}
	else if ((z < -std::abs(x)) && (z < -std::abs(y)))
	{
		return Down;
	}
	else
	{
		if (x > y && x <= -y)
			return Direction::South;
		if (x >= y && x > -y)
			return Direction::East;
		if (x < y && x >= -y)
			return Direction::North;
		if (x <= y && x < -y)
			return Direction::West;
		assert(x == 0 && y == 0);
		return Direction::None;
	}
}

Direction::Value Direction::From(HalfPiAngle angle)
{
	angle.Normalize();
	auto it = std::find_if(half_pi_angle.begin(), half_pi_angle.end(), [angle](const decltype(half_pi_angle)::value_type& kv)
	{
		return kv.second == angle;
	});
	return it->first;
}


Direction::Direction(const Position& desiredVector) :
	value(From(desiredVector))
{
}
    
Direction::Direction(int x, int y, int z) :
	value(From(Position(x,y),z))
{
}

Position Direction::Vector() const
{
	return Position(vector[value].x, vector[value].y);
}

double Direction::Angle() const
{
	auto it = half_pi_angle.find(value);
	if (it == half_pi_angle.end())
		return std::numeric_limits<float>::quiet_NaN();
	else
		return it->second.Angle();
}

Direction Direction::Opposite() const
{
	return Direction(Value(value^Negative));
}


bool Direction::IsOpposite(const Direction& other) const
{
	if ((other.value & Plane) != (value & Plane))
		return false;
	return (other.value & Negative) != (value & Negative);
}

bool Direction::IsClockwise(const Direction& other) const
{
	if ((!IsHorizontal()) || (!other.IsHorizontal()))
		return false;
	return HalfPiDelta(other).Normalize() == HalfPiAngle(-1);
}

bool Direction::IsCounterClockwise(const Direction& other) const
{
	if ((!IsHorizontal()) || (!other.IsHorizontal()))
		return false;
	return HalfPiDelta(other).Normalize() == HalfPiAngle(1);
}

HalfPiAngle Direction::HalfPiDelta(const Direction& other) const
{
	auto it = half_pi_angle.find(value);
	if (it == half_pi_angle.end())
		throw std::invalid_argument("Can't turn from vertical or unspecified direction");
	auto oit = half_pi_angle.find(other.value);
	if (oit == half_pi_angle.end())
		throw std::invalid_argument("Can't turn to vertical or unspeficied direction");
	return (it->second - oit->second);
}

bool Direction::IsProne() const
{
	return IsVertical();
}

bool Direction::IsVertical() const
{
	return 	(value&Vertical);
}

bool Direction::IsHorizontal() const
{
	return (value&Horizontal);
}

void Direction::Fall()
{
    value = Down;
}


Direction Direction::Turn(const Direction& turn) const
{
	if (turn.IsVertical())
	{
		return turn;
	}
	else if (IsVertical())
	{
		return *this;
	}
	else if (IsNone())
	{
		return turn;
	}
	else if (turn.IsNone())
	{
		return *this;
	}
	else
	{	// both this and turn are horizontal
		auto angle = (half_pi_angle.at(value) + half_pi_angle.at(turn.value)).Normalize();
		return Direction(From(angle));
	}
}

std::wstring Direction::AbsoluteDescription() const
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
    { Direction::Value::North, Position(0, 1) },
    { Direction::Value::East, Position(1, 0) },
	{ Direction::Value::South, Position(0, -1) },
	{ Direction::Value::West, Position(-1, 0) },
	{ Direction::Value::Up, Position(0, 0) },
	{ Direction::Value::Down, Position(0, 0) },
};

std::map<Direction::Value, HalfPiAngle> Direction::half_pi_angle=
{
	{ Direction::Value::North, HalfPiAngle(1) },
	{ Direction::Value::East, HalfPiAngle(0) },
	{ Direction::Value::South, HalfPiAngle(-1) },
	{ Direction::Value::West, HalfPiAngle(2) }
};

std::map<Direction::Value, std::wstring> Direction::description =
{
    { Direction::Value::None, L"" },
    { Direction::Value::North, L"North" },
    { Direction::Value::East, L"East" },
	{ Direction::Value::South, L"South" } ,
	{ Direction::Value::West, L"West" },
	{ Direction::Value::Up, L"Up" } ,
	{ Direction::Value::Down, L"Down" },
};

}    // ::Game

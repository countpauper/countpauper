#include "stdafx.h"
#include "Physics/Direction.h"
#include "Engine/Geometry.h"
#include "Engine/Vector.h"

namespace Physics
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
const Direction& Direction::x = east;
const Direction& Direction::y = north;
const Direction& Direction::z = up;

const std::map<const std::wstring_view, Direction> Direction::map(
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


std::array<Direction, 4> Direction::cardinal
{
    Direction::east,
    Direction::north,
    Direction::west,
    Direction::south
};

std::array<Direction, 6> Direction::all
{
    Direction::east,
    Direction::north,
    Direction::west,
    Direction::south,
    Direction::up,
    Direction::down
};

std::array<Direction, 3> Direction::positive
{
    Direction::east,
    Direction::north,
    Direction::up,
};

Direction::Value Direction::From(const Position& vector)
{
	auto x = vector.x;
    auto y = vector.y;
    auto z = vector.z;
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
	value(From(Position(x,y,z)))
{
}

Position Direction::Vector() const
{
	return Position(vector[value].x, vector[value].y, vector[value].z);
}


double Direction::Surface(const Engine::Vector& grid) const
{
    if (IsVertical())
    {
        assert(grid.x == grid.y);
        return grid.x * grid.z;
    }
    else
    {
        return grid.x* grid.y;
    }
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


bool Direction::IsPosititve() const
{
    return (value & Negative) == 0 && value != 0;
}

bool Direction::IsNegative() const
{
    return (value & Negative) != 0 && value != 0;
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

bool Direction::IsPerpendicular(const Direction& other) const
{
    return ((!IsNone()) && (!other.IsNone()) &&
        (value&Value::Axes) != (other.value&Value::Axes));
}

Direction Direction::Perpendicular(const Direction& other) const
{
    if (IsParallel(other))
        throw std::runtime_error("Can't determine one perpendicular axis to a parallel axis");
    if ((this->IsNone()) || (other.IsNone()))
        throw std::runtime_error("Can't determine an axis perpendicular to None");
    auto perpendicularAxis = Value::Axes & ~ (value | other.value);
    return Direction(perpendicularAxis);
}

bool Direction::IsParallel(const Direction& other) const
{
    return ((!IsNone()) && (!other.IsNone()) &&
        (value&Value::Axes) == (other.value&Value::Axes));
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

Direction Direction::Axis() const
{
    return Direction(value&Value::Axes);
}

bool Direction::IsX() const
{
    return (value & Value::XAxis);
}

bool Direction::IsY() const
{
    return (value & Value::YAxis);
}

bool Direction::IsZ() const
{
    return (value & Value::ZAxis);
}

bool Direction::IsValid() const
{
    auto axis = value & Value::Axes;
    if (axis == 0)
    {   // if no axis, then can't be negative
        return (value & Value::Negative) == 0;
    }
    else
    {   // only one axis
        return (axis == Value::XAxis || axis == Value::YAxis || axis == Value::ZAxis);
    }
}

int Direction::X() const
{
    return vector[value].x;
}

int Direction::Y() const
{
    return vector[value].y;
}

int Direction::Z() const
{
    return vector[value].z;
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

std::array<Position, 10> Direction::vector =
{ 
    Position(0, 0, 0), // None
    Position(),
    Position(1, 0, 0), // East
    Position(-1, 0, 0), // West
    Position(0, 1, 0), // North
    Position(0, -1, 0), // South
    Position(),
    Position(),
    Position(0, 0, 1), // Up
    Position(0, 0,-1) // Down
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


std::ostream& operator<<(std::ostream& os, const Direction& dir)
{
    os << Engine::from_string<std::string>(dir.AbsoluteDescription()).c_str();
    return os;
}

std::wostream& operator<<(std::wostream& os, const Direction& dir)
{
    os << dir.AbsoluteDescription().c_str();
    return os;
}

Directions Directions::none(0);

Directions Directions::all =
    Direction::north | 
    Direction::south | 
    Direction::east | 
    Direction::west | 
    Direction::up | 
    Direction::down;

Directions Directions::axes =
    Direction::x |
    Direction::y |
    Direction::z;

Directions::Directions() :
    Directions(0)
{
}

Directions::Directions(Direction dir) :
    Directions()
{
    (*this) |= dir;
}

Directions::Directions(uint16_t flags) :
    flags(flags)
{
}

Directions& Directions::operator|=(const Direction& dir)
{
    flags |= 1 << dir.Id();
    return *this;
}

bool Directions::operator==(const Directions& other) const
{
    return flags == other.flags;
}

bool Directions::operator[](const Direction& dir) const
{
    return (flags & (1 << dir.Id())) != 0;
}

bool Directions::empty() const
{
    return flags == 0;
}

size_t Directions::size() const
{
    size_t count = 0;
    for (int bit = 0; bit < sizeof(flags) * 8; ++bit)
    {
        if (flags&(1 << bit))
            ++count;
    }
    return count;
}

Directions::operator bool() const
{
    return !empty();
}

Directions::iterator::iterator(const uint16_t& flags, int start) : 
    bit(start -1),
    flags(flags)
{
    operator++();
}

Directions::iterator& Directions::iterator::operator++()
{
    while (++bit < sizeof(flags) * 8)
    {
        if (flags & (1<<bit))
            return *this;
    }
    return *this;
}

bool Directions::iterator::operator==(const Directions::iterator& other) const
{
    return bit == other.bit && flags == other.flags;
}

bool Directions::iterator::operator!=(const Directions::iterator& other) const
{
    return !((*this) == other);
}

Directions::iterator::value_type Directions::iterator::operator*() const
{
    return Direction(uint8_t(bit));
}

Directions::iterator Directions::begin() const
{
    return iterator(flags, 0);
}

Directions::iterator Directions::end() const
{
    return iterator(flags, sizeof(flags) * 8);
}

Directions& Directions::operator&=(const Directions& dirs)
{
    flags &= dirs.flags;
    return *this;
}

Directions& Directions::operator|=(const Directions& dirs)
{
    flags |= dirs.flags;
    return *this;
}

Directions& operator|(const Directions& a, const Directions& b)
{
    return Directions(a) |= b;
}
Directions& operator&(const Directions& a, const Directions& b)
{
    return Directions(a) &= b;
}


Directions operator|(const Direction& a, const Direction& b) 
{ 
    return Directions(a) |= b; 
}

Directions operator|(Directions dirs, const Direction& b) 
{ 
    return dirs |= b; 
}

}    // ::Game

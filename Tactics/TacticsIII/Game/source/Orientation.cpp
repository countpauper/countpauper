#include "Game/Orientation.h"
#include "Geometry/Geometry.h"
#include "Geometry/Vector.h"
#include "Geometry/Angles.h"
#include <assert.h>

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

Orientation::Orientation() :
	value(None)
{
}

Orientation::Orientation(Orientation::Value value) :
	value(value)
{
}

Orientation::Orientation(uint8_t id) :
    Orientation(static_cast<Value>(id))
{
}

uint8_t Orientation::Id() const
{
    return static_cast<uint8_t>(value);
}

const Orientation Orientation::none;
const Orientation Orientation::east(Orientation::East);
const Orientation Orientation::north(Orientation::North);
const Orientation Orientation::west(Orientation::West);
const Orientation Orientation::south(Orientation::South);
const Orientation Orientation::up(Orientation::Up);
const Orientation Orientation::down(Orientation::Down);
const Orientation& Orientation::forward = east;
const Orientation& Orientation::left = north;
const Orientation& Orientation::backward = west;
const Orientation& Orientation::right = south;
const Orientation& Orientation::x = east;
const Orientation& Orientation::y = north;
const Orientation& Orientation::z = up;

const std::map<const std::wstring_view, Orientation> Orientation::map(
{
	{ L"east", Orientation::east },
	{ L"north", Orientation::north },
	{ L"west", Orientation::west },
	{ L"south", Orientation::south },
	{ L"forward", Orientation::forward },
	{ L"backward", Orientation::backward },
	{ L"right", Orientation::right },
	{ L"left", Orientation::left },
	{ L"up",Orientation::up },
	{ L"down", Orientation::down },
});


std::array<Orientation, 4> Orientation::cardinal
{
    Orientation::east,
    Orientation::north,
    Orientation::west,
    Orientation::south
};

std::array<Orientation, 6> Orientation::all
{
    Orientation::east,
    Orientation::north,
    Orientation::west,
    Orientation::south,
    Orientation::up,
    Orientation::down
};

std::array<Orientation, 3> Orientation::positive
{
    Orientation::east,
    Orientation::north,
    Orientation::up,
};

Orientation::Value Orientation::From(const Engine::Position& vector)
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
			return Orientation::South;
		if (x >= y && x > -y)
			return Orientation::East;
		if (x < y && x >= -y)
			return Orientation::North;
		if (x <= y && x < -y)
			return Orientation::West;
		assert(x == 0 && y == 0);
		return Orientation::None;
	}
}

Orientation::Value Orientation::From(HalfPiAngle angle)
{
	angle.Normalize();
	auto it = std::find_if(half_pi_angle.begin(), half_pi_angle.end(), [angle](const decltype(half_pi_angle)::value_type& kv)
	{
		return kv.second == angle;
	});
	return it->first;
}


Orientation::Orientation(const Engine::Position& desiredVector) :
	value(From(desiredVector))
{
}

Orientation::Orientation(int x, int y, int z) :
	value(From(Engine::Position(x,y,z)))
{
}

Engine::Position Orientation::Vector() const
{
	return Engine::Position(vector[value].x, vector[value].y, vector[value].z);
}


double Orientation::Surface(const Engine::Vector& grid) const
{
    if (IsZ())
    {
        assert(grid.x == grid.y);
        return grid.x * grid.y;
    }
    else if (IsY())
    {
        return grid.x * grid.z;
    }
    else
    {
        return grid.y * grid.z;
    }
}

double Orientation::Angle() const
{
	auto it = half_pi_angle.find(value);
	if (it == half_pi_angle.end())
		return std::numeric_limits<float>::quiet_NaN();
	else
		return it->second.Angle();
}

Orientation Orientation::Opposite() const
{
	return Orientation(Value(value^Negative));
}


bool Orientation::IsPosititve() const
{
    return (value & Negative) == 0 && value != 0;
}

bool Orientation::IsNegative() const
{
    return (value & Negative) != 0 && value != 0;
}

bool Orientation::IsOpposite(const Orientation& other) const
{
	if ((other.value & Plane) != (value & Plane))
		return false;
	return (other.value & Negative) != (value & Negative);
}

bool Orientation::IsClockwise(const Orientation& other) const
{
	if ((!IsHorizontal()) || (!other.IsHorizontal()))
		return false;
	return HalfPiDelta(other).Normalize() == HalfPiAngle(-1);
}

bool Orientation::IsCounterClockwise(const Orientation& other) const
{
	if ((!IsHorizontal()) || (!other.IsHorizontal()))
		return false;
	return HalfPiDelta(other).Normalize() == HalfPiAngle(1);
}

bool Orientation::IsPerpendicular(const Orientation& other) const
{
    return ((!IsNone()) && (!other.IsNone()) &&
        (value&Value::Axes) != (other.value&Value::Axes));
}

Orientation Orientation::Perpendicular(const Orientation& other) const
{
    if (IsParallel(other))
        throw std::runtime_error("Can't determine one perpendicular axis to a parallel axis");
    if ((this->IsNone()) || (other.IsNone()))
        throw std::runtime_error("Can't determine an axis perpendicular to None");
    auto perpendicularAxis = Value::Axes & ~ (value | other.value);
    return Orientation(perpendicularAxis);
}

bool Orientation::IsParallel(const Orientation& other) const
{
    return ((!IsNone()) && (!other.IsNone()) &&
        (value&Value::Axes) == (other.value&Value::Axes));
}

HalfPiAngle Orientation::HalfPiDelta(const Orientation& other) const
{
	auto it = half_pi_angle.find(value);
	if (it == half_pi_angle.end())
		throw std::invalid_argument("Can't turn from vertical or unspecified direction");
	auto oit = half_pi_angle.find(other.value);
	if (oit == half_pi_angle.end())
		throw std::invalid_argument("Can't turn to vertical or unspeficied direction");
	return (it->second - oit->second);
}

bool Orientation::IsProne() const
{
	return IsVertical();
}

bool Orientation::IsVertical() const
{
	return 	(value&Vertical);
}

bool Orientation::IsHorizontal() const
{
	return (value&Horizontal);
}

Orientation Orientation::Axis() const
{
    return Orientation(value&Value::Axes);
}

bool Orientation::IsX() const
{
    return (value & Value::XAxis);
}

bool Orientation::IsY() const
{
    return (value & Value::YAxis);
}

bool Orientation::IsZ() const
{
    return (value & Value::ZAxis);
}

bool Orientation::IsValid() const
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

int Orientation::X() const
{
    return vector[value].x;
}

int Orientation::Y() const
{
    return vector[value].y;
}

int Orientation::Z() const
{
    return vector[value].z;
}

Orientation Orientation::Gravity()
{
    return Orientation(Down);
}


Orientation Orientation::Turn(const Orientation& turn) const
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
		return Orientation(From(angle));
	}
}

std::string Orientation::AbsoluteDescription() const
{
    return std::string(description.at(value));
}

bool Orientation::IsNone() const
{
    return value == Orientation::None;
}

bool Orientation::operator==(const Orientation& other) const
{
    return value == other.value;
}
bool Orientation::operator<(const Orientation& other) const
{
    return value < other.value;
}

std::array<Engine::Position, 10> Orientation::vector =
{
    Engine::Position(0, 0, 0), // None
    Engine::Position(),
    Engine::Position(1, 0, 0), // East
    Engine::Position(-1, 0, 0), // West
    Engine::Position(0, 1, 0), // North
    Engine::Position(0, -1, 0), // South
    Engine::Position(),
    Engine::Position(),
    Engine::Position(0, 0, 1), // Up
    Engine::Position(0, 0,-1) // Down
};

std::map<Orientation::Value, HalfPiAngle> Orientation::half_pi_angle=
{
	{ Orientation::Value::North, HalfPiAngle(1) },
	{ Orientation::Value::East, HalfPiAngle(0) },
	{ Orientation::Value::South, HalfPiAngle(-1) },
	{ Orientation::Value::West, HalfPiAngle(2) }
};

std::map<Orientation::Value, std::string_view> Orientation::description =
{
    { Orientation::Value::None, "" },
    { Orientation::Value::North, "North" },
    { Orientation::Value::East, "East" },
	{ Orientation::Value::South, "South" } ,
	{ Orientation::Value::West, "West" },
	{ Orientation::Value::Up, "Up" } ,
	{ Orientation::Value::Down, "Down" },
};


std::ostream& operator<<(std::ostream& os, const Orientation& dir)
{
    os << dir.AbsoluteDescription().c_str();
    return os;
}

std::wostream& operator<<(std::wostream& os, const Orientation& dir)
{
    os << dir.AbsoluteDescription().c_str();
    return os;
}

Directions Directions::none(0);

Directions Directions::all =
    Orientation::north |
    Orientation::south |
    Orientation::east |
    Orientation::west |
    Orientation::up |
    Orientation::down;

Directions Directions::axes =
    Orientation::x |
    Orientation::y |
    Orientation::z;

Directions::Directions() :
    Directions(0)
{
}

Directions::Directions(Orientation dir) :
    Directions()
{
    (*this) |= dir;
}

Directions::Directions(uint16_t flags) :
    flags(flags)
{
}

Directions::Directions(const std::initializer_list<Orientation>& dirs) :
    Directions()
{
    for (const auto& dir : dirs)
        (*this) |= dir;
}


Directions& Directions::operator|=(const Orientation& dir)
{
    flags |= 1 << dir.Id();
    return *this;
}

bool Directions::operator==(const Directions& other) const
{
    return flags == other.flags;
}

bool Directions::operator[](const Orientation& dir) const
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
    return Orientation(uint8_t(bit));
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


Directions operator|(const Orientation& a, const Orientation& b)
{
    return Directions(a) |= b;
}

Directions operator|(Directions dirs, const Orientation& b)
{
    return dirs |= b;
}

}    // ::Game

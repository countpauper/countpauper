#include "Geometry/Orientation.h"
#include "Geometry/Geometry.h"
#include "Geometry/Vector.h"
#include "Geometry/Angles.h"
#include <assert.h>

namespace Engine
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

const std::map<const std::string_view, Orientation> Orientation::map(
{
	{ "front", Orientation::front },
	{ "back", Orientation::back },
	{ "right", Orientation::right },
	{ "left", Orientation::left },
	{ "up",Orientation::up },
	{ "down", Orientation::down },
});


std::array<Orientation, 4> Orientation::horizontal
{
    Orientation::front,
    Orientation::back,
    Orientation::left,
    Orientation::right
};

std::array<Orientation, 6> Orientation::all
{
    Orientation::front,
    Orientation::back,
    Orientation::left,
    Orientation::right,
    Orientation::up,
    Orientation::down
};

std::array<Orientation, 3> Orientation::positive
{
    Orientation::front,
    Orientation::right,
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
			return Orientation::Right;
		if (x >= y && x > -y)
			return Orientation::Left;
		if (x < y && x >= -y)
			return Orientation::Front;
		if (x <= y && x < -y)
			return Orientation::Back;
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

Engine::Position Orientation::GetVector() const
{
	return Engine::Position(vector[value].x, vector[value].y, vector[value].z);
}


double Orientation::Surface(const Vector& grid) const
{
    if (IsZ())
    {
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

bool Orientation::IsOpposite(Orientation other) const
{
	if ((other.value & Plane) != (value & Plane))
		return false;
	return (other.value & Negative) != (value & Negative);
}

bool Orientation::IsClockwise(Orientation other) const
{
	if ((!IsHorizontal()) || (!other.IsHorizontal()))
		return false;
	return HalfPiDelta(other).Normalize() == HalfPiAngle(-1);
}

bool Orientation::IsCounterClockwise(Orientation other) const
{
	if ((!IsHorizontal()) || (!other.IsHorizontal()))
		return false;
	return HalfPiDelta(other).Normalize() == HalfPiAngle(1);
}

bool Orientation::IsPerpendicular(Orientation other) const
{
    return ((!IsNone()) && (!other.IsNone()) &&
        (value&Value::Axes) != (other.value&Value::Axes));
}

Orientation Orientation::Perpendicular(Orientation other) const
{
    if (IsParallel(other))
        throw std::runtime_error("Can't determine one perpendicular axis to a parallel axis");
    if ((this->IsNone()) || (other.IsNone()))
        throw std::runtime_error("Can't determine an axis perpendicular to None");
    auto perpendicularAxis = Value::Axes & ~ (value | other.value);
    return Orientation(perpendicularAxis);
}

bool Orientation::IsParallel(Orientation other) const
{
    return ((!IsNone()) && (!other.IsNone()) &&
        (value&Value::Axes) == (other.value&Value::Axes));
}

HalfPiAngle Orientation::HalfPiDelta(Orientation other) const
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


Orientation Orientation::Turn(Orientation turn) const
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
    auto it = description.find(value);
    if (it==description.end())
        return "<unknown orientation>";
    else
        return std::string(it->second);
}

bool Orientation::IsNone() const
{
    return value == Orientation::None;
}

bool Orientation::operator==(Orientation other) const
{
    return value == other.value;
}
bool Orientation::operator<(Orientation other) const
{
    return value < other.value;
}

const Orientation Orientation::none{};
const Orientation Orientation::front{Orientation::Front};
const Orientation Orientation::back{Orientation::Back};
const Orientation Orientation::left{Orientation::Left};
const Orientation Orientation::right{Orientation::Right};
const Orientation Orientation::up{Orientation::Up};
const Orientation Orientation::down{Orientation::Down};

std::array<Engine::Position, 10> Orientation::vector =
{
    Engine::Position(0, 0, 0), // None
    Engine::Position(),
    Engine::Position(0, 1, 0), // Front
    Engine::Position(0, -1, 0), // Back
    Engine::Position(1, 0, 0), // Right
    Engine::Position(-1, 0, 0), // Left
    Engine::Position(),
    Engine::Position(),
    Engine::Position(0, 0, 1), // Up
    Engine::Position(0, 0,-1) // Down
};

std::map<Orientation::Value, HalfPiAngle> Orientation::half_pi_angle=
{
	{ Orientation::Value::Front, HalfPiAngle(1) },
	{ Orientation::Value::Right, HalfPiAngle(0) },
	{ Orientation::Value::Back, HalfPiAngle(-1) },
	{ Orientation::Value::Left, HalfPiAngle(2) }
};

std::map<Orientation::Value, std::string_view> Orientation::description =
{
    { Orientation::Value::None, "" },
    { Orientation::Value::Negative, "-" },
    { Orientation::Value::Front, "Front" },
    { Orientation::Value::Right, "Right" },
    { Orientation::Value::Horizontal, "Horizontal" },
	{ Orientation::Value::Back, "Back" } ,
	{ Orientation::Value::Left, "Left" },
	{ Orientation::Value::Up, "Up" } ,
	{ Orientation::Value::Down, "Down" },
    { Orientation::Value::Vertical, "Vertical" }
};


std::ostream& operator<<(std::ostream& os, Orientation dir)
{
    os << dir.AbsoluteDescription().c_str();
    return os;
}

Orientations Orientations::none(0);

Orientations Orientations::all =
    Orientation::front |
    Orientation::back |
    Orientation::left |
    Orientation::right |
    Orientation::up |
    Orientation::down;

Orientations::Orientations() :
    Orientations(0)
{
}

Orientations::Orientations(Orientation dir) :
    Orientations()
{
    (*this) |= dir;
}

Orientations::Orientations(uint16_t flags) :
    flags(flags)
{
}

Orientations::Orientations(const std::initializer_list<Orientation>& dirs) :
    Orientations()
{
    for (const auto& dir : dirs)
        (*this) |= dir;
}

Orientations::Orientations(const Engine::Position& v) :
    Orientations()
{
    for (const auto& ori: all)
    {
        int x = ori.GetVector().x * v.x;
        int y = ori.GetVector().y * v.y;
        int z = ori.GetVector().z * v.z;
        if (x>0 || y>0 || z>0)
            (*this) |= ori;
    }
}


Orientations& Orientations::operator|=(Orientation dir)
{
    flags |= 1 << dir.Id();
    return *this;
}

bool Orientations::operator==(Orientations other) const
{
    return flags == other.flags;
}

bool Orientations::operator[](Orientation dir) const
{
    return (flags & (1 << dir.Id())) != 0;
}

bool Orientations::empty() const
{
    return flags == 0;
}

size_t Orientations::size() const
{
    size_t count = 0;
    for (int bit = 0; bit < sizeof(flags) * 8; ++bit)
    {
        if (flags&(1 << bit))
            ++count;
    }
    return count;
}

Orientations::operator bool() const
{
    return !empty();
}

Orientations::iterator::iterator(const uint16_t& flags, int start) :
    bit(start -1),
    flags(flags)
{
    operator++();
}

Orientations::iterator& Orientations::iterator::operator++()
{
    while (++bit < sizeof(flags) * 8)
    {
        if (flags & (1<<bit))
            return *this;
    }
    return *this;
}

bool Orientations::iterator::operator==(const Orientations::iterator& other) const
{
    return bit == other.bit && flags == other.flags;
}

bool Orientations::iterator::operator!=(const Orientations::iterator& other) const
{
    return !((*this) == other);
}

Orientations::iterator::value_type Orientations::iterator::operator*() const
{
    return Orientation(uint8_t(bit));
}

Orientations::iterator Orientations::begin() const
{
    return iterator(flags, 0);
}

Orientations::iterator Orientations::end() const
{
    return iterator(flags, sizeof(flags) * 8);
}

Orientations& Orientations::operator&=(Orientations dirs)
{
    flags &= dirs.flags;
    return *this;
}

Orientations& Orientations::operator|=(Orientations dirs)
{
    flags |= dirs.flags;
    return *this;
}

Orientations& operator|(Orientations a, Orientations b)
{
    return Orientations(a) |= b;
}
Orientations& operator&(Orientations a, Orientations b)
{
    return Orientations(a) &= b;
}


Orientations operator|(Orientation a, Orientation b)
{
    return Orientations(a) |= b;
}

Orientations operator|(Orientations dirs, Orientation b)
{
    return dirs |= b;
}

std::ostream& operator<<(std::ostream& os, Orientations oris)
{
    if (oris) {
        for(auto ori: oris)
            os << ori;
    } else {
        os << "none";
    }
    return os;
}
}    // ::Game

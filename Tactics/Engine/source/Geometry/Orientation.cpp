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

    // This is a fairly complex algorithm, when requiring each orientation gets an exactly even slice of grids
    // To do it, 6 sectors are defined as bits N(+Y) S(-Y) E(+X) W(-X) U(+Z) D(-Z) (order of the enum but not same value)
    // These sectors overlap on the diagonals. There are 18 diagonals:
    //  8 diagonals where abs(x) == abs(y) == abs(z) (well 4 but both side of the origin)
    //  4 horizontal where z == 0 and abs(x) == abs(y)
    //  4 vertical x == 0 an 4 where y == 0
    //  If a grid is assigned to all sectors it must be 0,0,0, direction none
    //  If a grid is assigned to one of 6 sectors, this is the result.
    //  If the grid is assigned to 2 or 3 sectors it's on a diagonal. These are divided evenly over the horizontal directions
    //  This is done clockwise based on the sign of X and Y +X+Y=North +X-Y=East -X-Y=South, -X+Y=West
    //  Up and down do not get diagonals, those sectors are smaller
    //  This is done with a lookup table

    enum Sector {
        None = 0,
        PosX = 1<<0,
        NegX = 1<<1,
        PosY = 1<<2,
        NegY = 1<<3,
        PosZ = 1<<4,
        NegZ = 1<<5
    };
    unsigned sectorSet=None;

    if (x >= abs(y) + abs(z))
        sectorSet |= PosX;
    if (-x >= abs(y) + abs(z))
        sectorSet |= NegX;
    if (y >= abs(x) + abs(z))
        sectorSet |= PosY;
    if (-y >= abs(x) + abs(z))
        sectorSet |= NegY;
    if (z >= abs(x) + abs(y))
        sectorSet |= PosZ;
    if (-z >= abs(x) + abs(y))
        sectorSet |= NegZ;
    static constexpr Orientation::Value assignedSector[64] =
    {
        // Up = 0
        // 0                     PosX                     NegX                     PosX NegX
        Orientation::Invalid,    Orientation::Right,      Orientation::Left,       Orientation::Invalid,        // 0
        Orientation::Front,      Orientation::Front,      Orientation::Left,       Orientation::Invalid,         // PosY
        Orientation::Back,       Orientation::Right,      Orientation::Back,       Orientation::Invalid,       // NegY
        Orientation::Invalid,    Orientation::Invalid,    Orientation::Invalid,    Orientation::Invalid,        // Pos NegX
        //// +Z
        Orientation::Up,         Orientation::Right,      Orientation::Left,       Orientation::Invalid,        // 0
        Orientation::Front,      Orientation::Front,      Orientation::Left,       Orientation::Invalid,         // PosY
        Orientation::Back,       Orientation::Right,      Orientation::Back,       Orientation::Invalid,       // NegY
        Orientation::Invalid,    Orientation::Invalid,    Orientation::Invalid,    Orientation::Invalid,        // Pos NegX
        ///  -Z
        Orientation::Down,       Orientation::Right,      Orientation::Left,       Orientation::Invalid,        // 0
        Orientation::Front,      Orientation::Front,      Orientation::Back,       Orientation::Invalid,        // PosY
        Orientation::Back,       Orientation::Right,      Orientation::Left,       Orientation::Invalid,        // NegY
        Orientation::Invalid,    Orientation::Invalid,    Orientation::Invalid,    Orientation::Invalid,        // Pos NegX
        /// +Z-Z
        Orientation::Invalid,    Orientation::Invalid,    Orientation::Invalid,    Orientation::Invalid,        // 0
        Orientation::Invalid,    Orientation::Invalid,    Orientation::Invalid,    Orientation::Invalid,        // PosY
        Orientation::Invalid,    Orientation::Invalid,    Orientation::Invalid,    Orientation::Invalid,        // NegY
        Orientation::Invalid,    Orientation::Invalid,    Orientation::Invalid,    Orientation::None            // PosY, NegY
    };

    unsigned sectors = 0;

    auto result = assignedSector[sectorSet];
    assert(result != Orientation::Invalid);
    return result;
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

bool Orientation::IsClockwise(Orientation to) const
{
	if ((!IsHorizontal()) || (!to.IsHorizontal()))
		return false;
	return HalfPiDeltaTo(to).Normalize() == HalfPiAngle(-1);
}

bool Orientation::IsCounterClockwise(Orientation to) const
{
	if ((!IsHorizontal()) || (!to.IsHorizontal()))
		return false;
	return HalfPiDeltaTo(to).Normalize() == HalfPiAngle(1);
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

HalfPiAngle Orientation::HalfPiDeltaTo(Orientation to) const
{
	auto it = half_pi_angle.find(value);
	if (it == half_pi_angle.end())
		throw std::invalid_argument("Can't turn from vertical or unspecified direction");
	auto toit = half_pi_angle.find(to.value);
	if (toit == half_pi_angle.end())
		throw std::invalid_argument("Can't turn to vertical or unspeficied direction");
	return (toit->second - it->second);
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

Orientation::operator bool() const
{
    return !IsNone();
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
    return value <= Orientation::Negative;
}

bool Orientation::operator==(Orientation other) const
{
    if (IsNone())
        return other.IsNone();
    else
        return value == other.value;
}

bool Orientation::operator<(Orientation other) const
{
    return value < other.value;
}


std::map<Orientation::Value, HalfPiAngle> Orientation::half_pi_angle=
{
	{ Orientation::Value::Front, HalfPiAngle(0) },
	{ Orientation::Value::Back, HalfPiAngle(2) },
	{ Orientation::Value::Right, HalfPiAngle(-1) },
	{ Orientation::Value::Left, HalfPiAngle(1) }
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

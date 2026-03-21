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
    if (value <= Value::MinValid && value >= Value::Invalid )
        throw std::invalid_argument("Orientation value is invalid");
}

Orientation::Orientation(int index) :
    Orientation(Value(index > 2 ? (2 - index) : index + 1))
{
}

int Orientation::Index() const
{
    return value < 0 ? 2 - static_cast<int>(value) : static_cast<int>(value - 1);
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
    Orientation::up,
    Orientation::down,
    Orientation::front,
    Orientation::back,
    Orientation::left,
    Orientation::right
};


std::array<Orientation, 3> Orientation::positive
{
    Orientation::up,
    Orientation::front,
    Orientation::right,
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
    static constexpr std::array<Position, 8> oriVector =
    {
        Position(0, 0, 0),  // None
        Position(0, 0, 1),  // Up
        Position(0, 1, 0),  // Front
        Position(1, 0, 0),  // Right
        Position(0, 0,-1),  // Down
        Position(0, -1, 0), // Back
        Position(-1, 0, 0), // Left
        Position()          // Invalid
    };
    auto idx = Index()+1;
    return oriVector[idx];
}


double Orientation::Surface(const Vector& grid) const
{
    if (IsZ())
    {
        return grid.X() * grid.Y();
    }
    else if (IsY())
    {
        return grid.X() * grid.Z();
    }
    else
    {
        return grid.Y() * grid.Z();
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
	return Orientation(Value(-value));
}

bool Orientation::IsPosititve() const
{
    return value > Value::None;
}

bool Orientation::IsNegative() const
{
    return value < Value::None;
}

bool Orientation::IsOpposite(Orientation other) const
{
	return bool(*this) && *this == -other;
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
    auto axis = Axis();
    auto otherAxis = other.Axis();
    if (!axis || !otherAxis)
        return false;
    return axis != otherAxis;
}

Orientation Orientation::Perpendicular(Orientation other) const
{
    auto axis = Axis();
    auto otherAxis = other.Axis();
    if (!axis)
        return axis;
    if (!otherAxis)
        return otherAxis;
    static constexpr Orientation::Value perpendicular[4][4]=
    {
        // Z                  Y                   X
        { Orientation::None,  Orientation::XAxis, Orientation::YAxis }, // Z
        { Orientation::XAxis, Orientation::None,  Orientation::ZAxis }, // Y
        { Orientation::YAxis, Orientation::ZAxis, Orientation::None  }  // X
    };
    return Orientation(perpendicular[axis.Index()][otherAxis.Index()]);
}

bool Orientation::IsParallel(Orientation other) const
{
    auto axis = Axis();
    auto otherAxis = other.Axis();
    if (!axis || !otherAxis)
        return false;
    return axis == otherAxis;
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
    return Axis().IsZ();
}

bool Orientation::IsHorizontal() const
{
	auto axis = Axis();
    return axis.IsX() || axis.IsY();
}

Orientation Orientation::Axis() const
{
    return Orientation(Value(std::abs(value)));
}

Orientation::operator bool() const
{
    return !IsNone();
}

bool Orientation::IsX() const
{
    return std::abs(value) == Value::XAxis;
}

bool Orientation::IsY() const
{
    return std::abs(value) == Value::YAxis;
}

bool Orientation::IsZ() const
{
    return std::abs(value) == Value::ZAxis;
}

int Orientation::X() const
{
    return GetVector().x;
}

int Orientation::Y() const
{
    return GetVector().y;
}

int Orientation::Z() const
{
    return GetVector().z;
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

std::string_view Orientation::Description() const
{
	static const std::array<std::string_view, 8> description =
    {
        "",
        "Up",
        "Front",
        "Right",
        "Down",
        "Back",
        "Left",
        "Invalid"
    };


    return description[Index() + 1];
}

bool Orientation::IsNone() const
{
    return value == Value::None;
}

bool Orientation::operator==(Orientation other) const
{
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

std::ostream& operator<<(std::ostream& os, Orientation dir)
{
    os << dir.Description();
    return os;
}

Orientations Orientations::none(0);

Orientations Orientations::all =
    Orientation::up |
    Orientation::front |
    Orientation::right |
    Orientation::down |
    Orientation::back |
    Orientation::left;

Orientations Orientations::axes =
    Orientation::up |
    Orientation::front |
    Orientation::right;

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
    flags |= 1 << (dir.Index()+1);
    return *this;
}

bool Orientations::operator==(Orientations other) const
{
    return flags == other.flags;
}

bool Orientations::operator[](Orientation dir) const
{
    return (flags & (1 << (dir.Index()+1))) != 0;
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
    return Orientation(bit-1);
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

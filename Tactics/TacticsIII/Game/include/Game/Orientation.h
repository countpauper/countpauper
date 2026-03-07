#pragma once

#include <map>
#include <array>
#include "Utility/from_string.h"
#include "Geometry/Position.h"

namespace Engine { struct Vector;  }
namespace Game
{

class HalfPiAngle
{
public:
	HalfPiAngle();
	explicit HalfPiAngle(int value);
	bool operator==(const HalfPiAngle& other) const;
	HalfPiAngle& operator+=(const HalfPiAngle& other);
	HalfPiAngle& operator-=(const HalfPiAngle& other);
	double Angle() const;
	HalfPiAngle& Normalize();
private:
	int value;
};
HalfPiAngle operator+(const HalfPiAngle& a, const HalfPiAngle& b);
HalfPiAngle operator-(const HalfPiAngle& a, const HalfPiAngle& b);

// Orientation represents an integer unit vector, so it's either a cardinal wind direction, up or down
class Orientation
{
public:
	Orientation();
    Orientation(int x, int y, int z);
    explicit Orientation(uint8_t id);
    explicit Orientation(const Engine::Position& vector);
    Engine::Position Vector() const;
    double Surface(const Engine::Vector& grid) const;  // in m^2
    double Angle() const;
	Orientation Opposite() const;
    bool IsPosititve() const;
    bool IsNegative() const;
    bool IsOpposite(Orientation other) const;
    bool IsClockwise(Orientation other) const;
    bool IsCounterClockwise(Orientation other) const;
    bool IsPerpendicular(Orientation other) const;
    Orientation Perpendicular(Orientation other) const;
    bool IsParallel(Orientation other) const;
    bool IsProne() const;
	bool IsHorizontal() const;
	bool IsVertical() const;

    Orientation Axis() const;
    bool IsX() const;
    bool IsY() const;
    bool IsZ() const;
    bool IsValid() const;

    int X() const;
    int Y() const;
    int Z() const;

	static Orientation Gravity();
	Orientation Turn(Orientation turn) const;
    std::string AbsoluteDescription() const;

    uint8_t Id() const;

    bool IsNone() const;    // operator bool leads to implicit conversion confusion with operator==
    bool operator==(Orientation other) const;
    bool operator!=(Orientation other) const { return !((*this) == other); }
    bool operator<(Orientation other) const;

	static const Orientation none;
	static const Orientation east;
    static const Orientation forward;
    static const Orientation west;
	static const Orientation backward;
    static const Orientation x;
    static const Orientation north;
	static const Orientation left;
	static const Orientation south;
	static const Orientation right;
    static const Orientation y;
	static const Orientation up;
	static const Orientation down;
    static const Orientation z;
	static const std::map<const std::wstring_view, Orientation> map;
    static std::array<Orientation, 4> cardinal;
    static std::array<Orientation, 6> all;
    static std::array<Orientation, 3> positive;
protected:
	enum Value
	{
		None = 0,
		Negative = 1,
        East = 2,   // defined before xaxis for debugger
        XAxis = East,
		Forward = East,
		West = XAxis | Negative,
		Backward = West,

        North = 4,
        YAxis = North,
		Left = North,
		South = YAxis | Negative,
		Right = South,
		Horizontal = (XAxis | YAxis),

        Up = 8,
        ZAxis = Up,
		Down = ZAxis | Negative,
		Vertical = ZAxis,
		Plane = Vertical | Horizontal,
        Axes = XAxis | YAxis | ZAxis,
	};
	Orientation(Value value);
	Value value;
	HalfPiAngle HalfPiDelta(Orientation other) const;
	static Value From(const Engine::Position& vector);
	static Value From(HalfPiAngle angle);

	static std::map<Value, std::string_view> description;
    static std::array<Engine::Position,10> vector;
	static std::map<Value, HalfPiAngle> half_pi_angle;
};

std::ostream& operator<<(std::ostream& os, Orientation dir);

class Orientations
{
public:
    Orientations();
    explicit Orientations(Orientation dir);
    explicit Orientations(const std::initializer_list<Orientation>& dirs);
    explicit Orientations(uint16_t flags);
    explicit Orientations(const Engine::Position& v);
    Orientations& operator|=(Orientation dir);
    Orientations& operator&=(Orientations dirs);
    Orientations& operator|=(Orientations dirs);
    bool operator==(Orientations other) const;
    bool operator!=(Orientations other) const { return !(*this == other); }
    bool operator[](Orientation dir) const;
    bool empty() const;
    size_t size() const;

    explicit operator bool() const;
    static Orientations none;
    static Orientations all;
    static Orientations axes;

    class iterator
    {
    public:
        iterator(const uint16_t& flags, int bit);
        iterator& operator++();
        //iterator operator++(int);
        bool operator==(const iterator& other) const;
        bool operator!=(const iterator& other) const;
        using value_type = Orientation;
        value_type operator*() const;

        // iterator traits
        using difference_type = int;
        using iterator_category = std::forward_iterator_tag;
        using pointer = const Orientation*;   // BS, but needed to use Orientations in std::algorithms
        using reference = Orientation; // BS, but needed to use Orientations in std::algorithms

        int bit;
        const uint16_t& flags;
    };
    iterator begin() const;
    iterator end() const;

private:
    uint16_t flags;
};

Orientations operator|(Orientation a, Orientation b);
Orientations operator|(Orientations dirs, Orientation b);
std::ostream& operator<<(std::ostream& os, Orientations dirs);


}    // ::Game

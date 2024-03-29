#pragma once

#include <map>
#include <array>
#include "Engine/from_string.h"
#include "Position.h"

namespace Engine { struct Vector;  }
namespace Physics
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


// TODO: rename orientation


// Direction represents an integer unit vector, so it's either a cardinal wind direction, up or down 
class Direction
{
public:
	Direction();
    Direction(int x, int y, int z);
    explicit Direction(uint8_t id);
    explicit Direction(const Position& vector);
    Position Vector() const;
    double Surface(const Engine::Vector& grid) const;  // in m^2
    double Angle() const;
	Direction Opposite() const;
    bool IsPosititve() const;
    bool IsNegative() const;
    bool IsOpposite(const Direction& other) const;
    bool IsClockwise(const Direction& other) const;
    bool IsCounterClockwise(const Direction& other) const;
    bool IsPerpendicular(const Direction& other) const;
    Direction Perpendicular(const Direction& other) const;
    bool IsParallel(const Direction& other) const;
    bool IsProne() const;
	bool IsHorizontal() const;
	bool IsVertical() const;

    Direction Axis() const;
    bool IsX() const;
    bool IsY() const;
    bool IsZ() const;
    bool IsValid() const;

    int X() const;
    int Y() const;
    int Z() const;


	void Fall();
	Direction Turn(const Direction& turn) const;
    std::wstring AbsoluteDescription() const;

    uint8_t Id() const;

    bool IsNone() const;    // operator bool leads to implicit conversion confusion with operator==
    bool operator==(const Direction& other) const;
    bool operator!=(const Direction& other) const { return !((*this) == other); }
    bool operator<(const Direction& other) const;

	static const Direction none;
	static const Direction east;
    static const Direction& forward;
    static const Direction west;
	static const Direction& backward;
    static const Direction& x;
    static const Direction north;
	static const Direction& left;
	static const Direction south;
	static const Direction& right;
    static const Direction& y;
	static const Direction up;
	static const Direction down;
    static const Direction& z;
	static const std::map<const std::wstring_view, Direction> map;
    static std::array<Direction, 4> cardinal;
    static std::array<Direction, 6> all;
    static std::array<Direction, 3> positive;
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
	Direction(Value value);
	Value value;
	HalfPiAngle HalfPiDelta(const Direction& other) const;
	static Value From(const Position& vector);
	static Value From(HalfPiAngle angle);

	static std::map<Value, std::wstring> description;
    static std::array<Position,10> vector;
	static std::map<Value, HalfPiAngle> half_pi_angle;
};

std::ostream& operator<<(std::ostream& os, const Direction& dir);
std::wostream& operator<<(std::wostream& os, const Direction& dir);

class Directions
{
public:
    Directions();
    explicit Directions(Direction dir);
    explicit Directions(const std::initializer_list<Direction>& dirs);
    explicit Directions(uint16_t flags);

    Directions& operator|=(const Direction& dir);
    Directions& operator&=(const Directions& dirs);
    Directions& operator|=(const Directions& dirs);
    bool operator==(const Directions& other) const;
    bool operator!=(const Directions& other) const { return !(*this == other); }
    bool operator[](const Direction& dir) const;
    bool empty() const;
    size_t size() const;

    operator bool() const;
    static Directions none;
    static Directions all;
    static Directions axes;

    class iterator
    {
    public:
        iterator(const uint16_t& flags, int bit);
        iterator& operator++();
        //iterator operator++(int);
        bool operator==(const iterator& other) const;
        bool operator!=(const iterator& other) const;
        using value_type = Direction;
        value_type operator*() const;

        // iterator traits
        using difference_type = int;
        using iterator_category = std::forward_iterator_tag;
        using pointer = const Direction*;   // BS, but needed to use directions in std::algorithms
        using reference = const Direction&; // BS, but needed to use directions in std::algorithms

        int bit;
        const uint16_t& flags;
    };
    iterator begin() const;
    iterator end() const;

private:
    uint16_t flags;
};

Directions operator|(const Direction& a, const Direction& b);
Directions operator|(Directions dirs, const Direction& b);


}    // ::Game
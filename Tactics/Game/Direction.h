#pragma once

#include <map>
#include <array>

#include "Position.h"

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
    unsigned SurfaceEl() const;  
    double Surface() const;  // in m^2
    double Angle() const;
	Direction Opposite() const;
    bool IsPosititve() const;
    bool IsNegative() const;
    bool IsOpposite(const Direction& other) const;
    bool IsClockwise(const Direction& other) const;
    bool IsCounterClockwise(const Direction& other) const;
	bool IsProne() const;
	bool IsHorizontal() const;
	bool IsVertical() const;
	void Fall();
	Direction Turn(const Direction& turn) const;
    std::wstring AbsoluteDescription() const;

    uint8_t Id() const;

    bool IsNone() const;    // operator bool leads to implicit conversion confusion with operator==
    bool operator==(const Direction& other) const;
    bool operator<(const Direction& other) const;

	static const Direction none;
	static const Direction east;
	static const Direction& forward;
	static const Direction west;
	static const Direction& backward;
	static const Direction north;
	static const Direction& left;
	static const Direction south;
	static const Direction& right;
	static const Direction up;
	static const Direction down;
	static const std::map<const std::wstring, Direction> map;
    static std::array<Direction, 4> cardinal;
    static std::array<Direction, 6> all;
    static std::array<Direction, 3> positive;
protected:
	enum Value
	{
		None = 0,
		Negative = 1,
		East = 2,
		Forward = East,
		West = East | Negative,
		Backward = West,
		Parallel = (Forward | Backward) & ~Negative,

		North = 4,
		Left = North,
		South = North | Negative,
		Right = South,
		Lateral = (Left | Right) & ~Negative,

		Horizontal = (Parallel | Lateral),
		Up = 8,
		Down = Up | Negative,
		Vertical = (Up|Down) & ~Negative,
		Plane = Vertical | Horizontal
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


static inline std::ostream& operator<< (std::ostream& os, const Direction& dir)
{
    os << dir.AbsoluteDescription().c_str();
    return os;
}

class Directions
{
public:
    Directions();
    explicit Directions(Direction dir);
    explicit Directions(uint16_t flags);

    Directions& operator|=(const Direction& dir);
    Directions& operator&=(const Directions& dirs);
    Directions& operator|=(const Directions& dirs);
    bool operator[](const Direction& dir) const;
    bool empty() const;
    operator bool() const;
    static Directions all;

    class iterator
    {
    public:
        iterator(const uint16_t& flags, int bit);
        iterator& operator++();
        //iterator operator++(int);
        bool operator==(const iterator& other) const;
        bool operator!=(const iterator& other) const;
        using value_type = const Direction;
        value_type operator*() const;

        // iterator traits
        using difference_type = int;
        using iterator_category = std::forward_iterator_tag;
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
#pragma once
#include "Position.h"
#include "Size.h"
#include "Engine/Range.h"

namespace Engine
{

class Box
{
public:
    Box() : Box(Size()) {} 
    Box(Range<int> x, Range<int> y, Range<int> z);
    Box(Position from, Position to);
    Box(Position from, Size extent) : Box(from, from + extent) {}
    explicit Box(Size extent) : Box(Position(0, 0, 0), extent) {}
    Position Start() const;
    Position End() const;
    int Volume() const;
    Size Extent() const;

    operator bool() const { return !Empty(); }
    bool Empty() const;
    bool Contains(Position position) const;
    bool operator[](Position position) { return Contains(position); }
    Box& Grow(int amount);
    Box& Grow(Size amount);
    Box& operator+=(Size amount) { return Grow(amount); }
    Box& operator+=(Position translation);
    Position Clip(Position position) const;
    Box& operator|=(Position p);
    Box& operator|=(Box b);

    Range<int> x;
    Range<int> y;
    Range<int> z;


    static const Box empty;
    static const Box all;
};

Box operator|(Box a, Box b);
Box operator&(Box a, Box b);
Box operator|(Box a, Position p);
Box operator+(Box a, Size s);
Box operator+(Box a, Position translation);
std::ostream& operator<<(std::ostream& os, Box box);
std::wostream& operator<<(std::wostream& os, Box box);

}   // ::Engine


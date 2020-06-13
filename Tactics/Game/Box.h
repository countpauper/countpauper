#pragma once
#include "Position.h"
#include "Engine/Range.h"

namespace Game
{

class Box
{
public:
    Box(const Engine::Range<int>& x, const Engine::Range<int>& y, const Engine::Range<int>& z);
    Box(const Position& from, const Position& to);
    Box(const Position& from, const Size& extent) : Box(from, from + extent) {} 
    Box(const Size& extent) : Box(Position(0, 0, 0), extent) {}
    Position Start() const;
    Position End() const;
    int Volume() const;
    Size Extent() const;

    bool Contains(const Position& position) const;
    bool operator[](const Position& position) { return Contains(position); }
    Box& Grow(int amount);
    Box& Grow(const Size& amount);
    Box& operator+=(const Size& amount) { return Grow(amount); }
    Box& operator+=(const Position& translation);
    Position Clip(const Position& position) const;
    Box& operator|=(const Position& p);

    Engine::Range<int> x;
    Engine::Range<int> y;
    Engine::Range<int> z;
};

Box operator|(const Box& a, const Box& b);
Box operator&(const Box& a, const Box& b);
Box operator|(const Box& a, const Position& p);
Box operator+(const Box& a, const Size& s);
std::ostream& operator<<(std::ostream& os, const Box& box);
std::wostream& operator<<(std::wostream& os, const Box& box);

}   // ::Game


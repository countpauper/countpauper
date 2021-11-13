#include "stdafx.h"
#include "Box.h"

namespace Physics
{

const Box Box::empty(Engine::Range<int>(0,0), Engine::Range<int>(0,0), Engine::Range<int>(0,0));
const Box Box::all(Engine::Range<int>(std::numeric_limits<int>::min(), std::numeric_limits<int>::max()),
    Engine::Range<int>(std::numeric_limits<int>::min(), std::numeric_limits<int>::max()),
    Engine::Range<int>(std::numeric_limits<int>::min(), std::numeric_limits<int>::max()));


Box::Box(const Engine::Range<int>& x, const Engine::Range<int>& y, const Engine::Range<int>& z) :
    x(x),
    y(y),
    z(z)
{
}
Box::Box(const Position& from, const Position& to) :
    x(from.x, to.x),
    y(from.y, to.y),
    z(from.z, to.z)
{
}

Position Box::Start() const 
{ 
    return Position(x.begin, y.begin, z.begin); 
}
Position Box::End() const 
{ 
    return Position(x.end, y.end, z.end); 
}

bool Box::Empty() const
{
    return !(x && y && z);
}

int Box::Volume() const
{
    return Extent().Volume();
}

Size Box::Extent() const
{
    return Size(x.Size(), y.Size(), z.Size());
}

bool Box::Contains(const Position& position) const
{
    return x[position.x] &&
        y[position.y] &&
        z[position.z];
}
Box& Box::Grow(int amount)
{
    Grow(Size(amount, amount, amount));
    return *this;
}

Box& Box::Grow(const Size& amount)
{
    x.Grow(amount.x);
    y.Grow(amount.y);
    z.Grow(amount.z);
    return *this;
}

Box& Box::operator+=(const Position& translation)
{
    x += translation.x;
    y += translation.y;
    z += translation.z;
    return *this;
}

Position Box::Clip(const Position& position) const
{
    return Position(x.Clip(position.x),
        y.Clip(position.y),
        z.Clip(position.z));
}

Box& Box::operator|=(const Position& p)
{
    x |= p.x;
    y |= p.y;
    z |= p.z;
    return *this;
}
Box operator|(const Box& a, const Box& b)
{
    return Box(
        a.x | b.x,
        a.y | b.y,
        a.z | b.z);
}

Box operator&(const Box& a, const Box& b)
{
    return Box(
        a.x & b.x,
        a.y & b.y,
        a.z & b.z);
}

Box operator|(const Box& a, const Position& p)
{
    return Box(a) |= p;
}

Box operator+(const Box& a, const Size& s)
{
    return Box(a) += s;
}

Box operator+(const Box& a, const Position& translation)
{
    return Box(a) += translation;
}

std::ostream& operator<<(std::ostream& os, const Box& box)
{
    os << box.Start() << "-" << box.End();
    return os;
}
std::wostream& operator<<(std::wostream& os, const Box& box)
{
    os << box.Start() << "-" << box.End();
    return os;
}
}

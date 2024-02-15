#include "Engine/Box.h"

namespace Engine
{

const Box Box::empty(Range<int>(0,0), Range<int>(0,0), Range<int>(0,0));
const Box Box::all(Range<int>(std::numeric_limits<int>::min(), std::numeric_limits<int>::max()),
    Range<int>(std::numeric_limits<int>::min(), std::numeric_limits<int>::max()),
    Range<int>(std::numeric_limits<int>::min(), std::numeric_limits<int>::max()));


Box::Box(Range<int> x, Range<int> y, Range<int> z) :
    x(x),
    y(y),
    z(z)
{
}
Box::Box(Position from, Position to) :
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

bool Box::Contains(Position position) const
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

Box& Box::Grow(Size amount)
{
    x.Grow(amount.x);
    y.Grow(amount.y);
    z.Grow(amount.z);
    return *this;
}

Box& Box::operator+=(Position translation)
{
    x += translation.x;
    y += translation.y;
    z += translation.z;
    return *this;
}

Position Box::Clip(Position position) const
{
    return Position(x.Clip(position.x),
        y.Clip(position.y),
        z.Clip(position.z));
}

Box& Box::operator|=(Position p)
{
    x |= p.x;
    y |= p.y;
    z |= p.z;
    return *this;
}

Box& Box::operator|=(Box b)
{
    x |= b.x;
    y |= b.y;
    z |= b.z;
    return *this;
}

Box operator|(Box a, Box b)
{
    return Box(a) |= b;
}

Box operator&(Box a, Box b)
{
    return Box(
        a.x & b.x,
        a.y & b.y,
        a.z & b.z);
}

Box operator|(Box a, Position p)
{
    return Box(a) |= p;
}

Box operator+(Box a, Size s)
{
    return Box(a) += s;
}

Box operator+(Box a, Position translation)
{
    return Box(a) += translation;
}

std::ostream& operator<<(std::ostream& os, Box box)
{
    os << box.Start() << "-" << box.End();
    return os;
}
std::wostream& operator<<(std::wostream& os, Box box)
{
    os << box.Start() << "-" << box.End();
    return os;
}
}

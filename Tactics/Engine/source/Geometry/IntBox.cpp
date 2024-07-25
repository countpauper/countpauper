#include "Geometry/IntBox.h"
#include <cassert>

namespace Engine
{



IntBox::IntBox(Range<int> x, Range<int> y, Range<int> z) :
    x(x),
    y(y),
    z(z)
{
}
IntBox::IntBox(Position from, Position to) :
    x(from.x, to.x),
    y(from.y, to.y),
    z(from.z, to.z)
{
}

Position IntBox::Start() const
{
    return Position(x.begin, y.begin, z.begin);
}
Position IntBox::End() const
{
    return Position(x.end, y.end, z.end);
}

bool IntBox::Empty() const
{
    return !(x && y && z);
}

int IntBox::Volume() const
{
    return Extent().Volume();
}

int IntBox::Width() const
{
    return x.Size();
}

int IntBox::Height() const
{
    return y.Size();
}

int IntBox::Depth() const
{
    return z.Size();
}

Size IntBox::Extent() const
{
    return Size(x.Size(), y.Size(), z.Size());
}

bool IntBox::Contains(Position position) const
{
    return x[position.x] &&
        y[position.y] &&
        z[position.z];
}
IntBox& IntBox::Grow(int amount)
{
    Grow(Size(amount, amount, amount));
    return *this;
}

IntBox& IntBox::Grow(Size amount)
{
    x.Grow(amount.x);
    y.Grow(amount.y);
    z.Grow(amount.z);
    return *this;
}

IntBox& IntBox::operator+=(Position translation)
{
    x += translation.x;
    y += translation.y;
    z += translation.z;
    return *this;
}

Position IntBox::Clip(Position position) const
{
    return Position(x.Clip(position.x),
        y.Clip(position.y),
        z.Clip(position.z));
}

IntBox::Iterator IntBox::begin() const
{
    return Iterator(*this, 0);
}

IntBox::Iterator IntBox::end() const
{
    return Iterator(*this, Volume());
}

IntBox::Iterator& IntBox::Iterator::operator++()
{
    index++;
    return *this;
}

IntBox::Iterator IntBox::Iterator::operator++(int)
{
    IntBox::Iterator copy = *this;
    ++index;
    return copy;
}

IntBox::Iterator& IntBox::Iterator::operator--()
{
    index--;
    return *this;
}

IntBox::Iterator IntBox::Iterator::operator--(int)
{
    IntBox::Iterator copy = *this;
    --index;
    return copy;
}

bool IntBox::Iterator::operator==(const IntBox::Iterator& rhs) const
{
    assert(&box == &rhs.box);
    return index == rhs.index;
}

bool IntBox::Iterator::operator!=(const IntBox::Iterator& rhs) const
{
    assert(&box == &rhs.box);
    return index != rhs.index;
}

Position IntBox::Iterator::operator*() const
{

    Position offset{
        index % box.Width(),
        index / box.Width() % box.Height(),
        index / (box.Width() * box.Height()) % box.Depth()
    };
    return box.Start() + offset;
}

std::ptrdiff_t IntBox::Iterator::operator-(const IntBox::Iterator& rhs)
{
    assert(&box == &rhs.box);
    return index - rhs.index;
}

IntBox::Iterator::Iterator(const IntBox& box, int index) :
    box(box),
    index(index)
{
}

IntBox& IntBox::operator|=(Position p)
{
    x |= p.x;
    y |= p.y;
    z |= p.z;
    return *this;
}

IntBox& IntBox::operator|=(IntBox b)
{
    x |= b.x;
    y |= b.y;
    z |= b.z;
    return *this;
}

IntBox operator|(IntBox a, IntBox b)
{
    return IntBox(a) |= b;
}

IntBox operator&(IntBox a, IntBox b)
{
    return IntBox(
        a.x & b.x,
        a.y & b.y,
        a.z & b.z);
}

IntBox operator|(IntBox a, Position p)
{
    return IntBox(a) |= p;
}

IntBox operator+(IntBox a, Size s)
{
    return IntBox(a) += s;
}

IntBox operator+(IntBox a, Position translation)
{
    return IntBox(a) += translation;
}

std::ostream& operator<<(std::ostream& os, IntBox box)
{
    os << box.Start() << "-" << box.End();
    return os;
}
std::wostream& operator<<(std::wostream& os, IntBox box)
{
    os << box.Start() << "-" << box.End();
    return os;
}
}

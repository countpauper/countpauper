#pragma once
#include "Geometry/Position.h"
#include "Geometry/Size.h"
#include "Utility/Range.h"

namespace Engine
{

class IntBox
{
public:
    IntBox() : IntBox(Size()) {}
    IntBox(Range<int> x, Range<int> y, Range<int> z);
    IntBox(Position from, Position to);
    IntBox(Position from) : IntBox(from, from) {}
    IntBox(Position from, Size extent) : IntBox(from, from + extent) {}
    explicit IntBox(Size extent) : IntBox(Position(0, 0, 0), extent) {}
    Position Start() const;
    Position End() const;
    int Volume() const;
    int Width() const;
    int Height() const;
    int Depth() const;
    Size Extent() const;
    class Iterator
    {
    public:
        using difference_type = std::ptrdiff_t;
        using value_type = Position;
        using pointer = value_type*;
        using reference	 = value_type&;
        using iterator_category = std::bidirectional_iterator_tag;  // TODO: could be random access if += and + are defined

        Iterator& operator++();
        Iterator operator++(int);
        Iterator& operator--();
        Iterator operator--(int);
        bool operator==(const Iterator& rhs) const;
        bool operator!=(const Iterator& rhs) const;
        value_type operator*() const;
        difference_type operator-(const Iterator& rhs);
    private:
        friend class IntBox;
        Iterator(const IntBox& box, int index);
        const IntBox& box;
        int index;
    };
    Iterator begin() const;
    Iterator end() const;

    operator bool() const { return !Empty(); }
    bool Empty() const;
    bool Contains(Position position) const;
    bool operator[](Position position) { return Contains(position); }
    IntBox& Grow(int amount);
    IntBox& Grow(Size amount);
    IntBox& operator+=(Size amount) { return Grow(amount); }
    IntBox& operator+=(Position translation);
    Position Clip(Position position) const;
    IntBox& operator|=(Position p);
    IntBox& operator|=(IntBox b);

    Range<int> x;
    Range<int> y;
    Range<int> z;


    static const IntBox empty;
    static const IntBox all;
};

IntBox operator|(IntBox a, IntBox b);
IntBox operator&(IntBox a, IntBox b);
IntBox operator|(IntBox a, Position p);
IntBox operator+(IntBox a, Size s);
IntBox operator+(IntBox a, Position translation);
std::ostream& operator<<(std::ostream& os, IntBox box);
std::wostream& operator<<(std::wostream& os, IntBox box);

}   // ::Engine


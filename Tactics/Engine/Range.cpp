#include "stdafx.h"
#include "Range.h"

namespace Engine
{
Range::Range(double begin, double end) :
    begin(begin),
    end(end)
{
}

Range::operator bool() const
{
    return Size() != 0;
}
double Range::Size() const
{
    return end - begin;
}

Range operator|(const Range& a, const Range& b)
{
    return Range(std::min(a.begin, b.begin), std::max(a.end, b.end));
}

Range operator&(const Range& a, const Range& b)
{
    auto begin = std::max(a.begin, b.begin);
    return Range(begin, std::max(begin, std::min(a.end, b.end)));
}

}

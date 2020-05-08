#pragma once
#pragma once

#include "Coordinate.h"
namespace Engine
{

template<typename T>
struct Range
{
    Range() = default;
    Range(T begin, T end) :
        begin(begin),
        end(end)
    {
    }

    operator bool() const
    {
        return end > begin;
    }
    T Size() const
    {
        return end - begin;
    }

    T begin;
    T end;
};

template<typename T>
Range<T> operator|(const Range<T>& a, const Range<T>& b)
{
    return Range(std::min(a.begin, b.begin), std::max(a.end, b.end));
}

template<typename T>
Range<T> operator&(const Range<T>& a, const Range<T>& b)
{
    auto begin = std::max(a.begin, b.begin);
    return Range(begin, std::max(begin, std::min(a.end, b.end)));
}


}

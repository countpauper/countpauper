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

    bool operator[](T v) const
    {   // in
        return (begin <= v) && (v <= end);
    }

    Range& operator|=(T v)
    {
        begin = std::min(begin, v);
        end = std::max(end, v);
        return *this;
    }

    Range& operator+=(T v)
    {
        begin += v;
        end += v;
        return *this;
    }

    Range& Expand(T v)
    {
        if (v > 0)
            end += v;
        else
            begin += v;
        return *this;
    }

    Range& Grow(T v)
    {
        end += v;
        begin -= v;
        return *this;
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
Range<T> operator|(const Range<T>& a, T v)
{
    return Range(a)|=v;
}

template<typename T>
Range<T> operator+(const Range<T>& a, T v)
{
    return Range(a) += v;
}

template<typename T>
Range<T> operator&(const Range<T>& a, const Range<T>& b)
{
    auto begin = std::max(a.begin, b.begin);
    return Range(begin, std::max(begin, std::min(a.end, b.end)));
}


}

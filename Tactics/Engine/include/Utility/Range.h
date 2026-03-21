#pragma once

#include "Geometry/Coordinate.h"
#include "Utility/Maths.h"
#include <algorithm>
#include <type_traits>

namespace Engine
{

// range [begin, end>
template<typename T>
struct Range
{
    constexpr Range() = default;

    constexpr Range(T begin, T end) :
        begin(begin),
        end(end)
    {
    }

    template<typename OT>
    constexpr explicit Range(Range<OT> o) :
        begin(static_cast<T>(o.begin)),
        end(static_cast<T>(o.end))
    {
    }

    constexpr T ElementSize() const
    {   // floating point ranges are inclusive for clipping
        if (std::is_floating_point<T>::value)
            return static_cast<T>(0.0);
        else
            return static_cast<T>(1);
    }

    explicit constexpr operator bool() const
    {
        return !IsEmpty();
    }

    constexpr bool IsEmpty() const
    {
        return end <= begin;
    }

    constexpr T Size() const
    {
        return end - begin;
    }

    constexpr bool operator[](T v) const
    {   // in
        return (begin <= v) && (v+ElementSize() <= end);
    }

    constexpr Range& operator|=(T v)
    {
        begin = std::min(begin, v);
        end = std::max(end, v+ElementSize());
        return *this;
    }


    constexpr Range& operator+=(T v)
    {
        begin += v;
        end += v;
        return *this;
    }
    constexpr Range& operator-=(T v) { return operator+=(-v); }
    constexpr Range& operator*=(T v)
    {
        begin *= v;
        end *= v;
        return *this;
    }

    constexpr Range& operator/=(T v)
    {
        return operator*=(1 / v);
    }

    constexpr Range& operator|=(const Range<T>& o)
    {
        *this = *this | o;
        return *this;
    }

    constexpr Range& operator&=(const Range<T>& o)
    {
        *this = *this & o;
        return *this;
    }

    constexpr bool operator<(T v) const
    {
        return end <= v;
    }
    constexpr bool operator<=(T v) const
    {
        return begin <= v;
    }
    constexpr bool operator>(T v) const
    {
        return begin > v;
    }
    constexpr bool operator>=(T v) const
    {
        return end > v;
    }

    constexpr bool operator==(const Range<T>& o) const
    {
        return begin == o.begin && end == o.end;
    }

    constexpr Range& Expand(T v)
    {
        if (v > 0)
            end += v;
        else
            begin += v;
        return *this;
    }

    constexpr Range& Grow(T v)
    {
        end += v;
        begin -= v;
        return *this;
    }
    constexpr T Clip(T v) const
    {
        return Engine::Clip(v, begin, end - ElementSize());
    }

    constexpr T Middle() const
    {
        return Middle(std::is_floating_point<T>());
    }

    constexpr T Middle(std::true_type) const
    {
        if ((std::isinf(end)) && (std::isinf(begin)) && (std::signbit(begin)!=std::signbit(end)))
            return 0;
        else
            return (begin + end) / 2;
    }

    constexpr T Middle(std::false_type) const
    {
        return (begin + end) / 2;
    }


    T begin;
    T end;


    constexpr bool infinite() const
    {
        return std::isinf(begin) || std::isinf(end);
    }

    constexpr bool finite() const
    {
        return !infinite();
    }

    constexpr bool IsNumbers() const
    {
        return !std::isnan(begin) &&
            !std::isnan(end);
    }

    constexpr T GetNumber() const
    {
        return std::isnan(begin) ? end : begin;
    }

    static constexpr Range infinity()
    {
        static_assert(std::numeric_limits<T>::has_infinity);
        return Range(-std::numeric_limits<T>::infinity(), std::numeric_limits<T>::infinity());
    }

    static constexpr Range max()
    {
        return Range(std::numeric_limits<T>::min(), std::numeric_limits<T>::max());
    }

    static constexpr Range null()
    {
        return Range(0, 0);
    }

    static constexpr Range empty()
    {
        return Range(std::numeric_limits<T>::max(), std::numeric_limits<T>::min());
    }
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
Range<T> operator-(const Range<T>& a, T v)
{
    return Range(a) -= -v;
}

template<typename T>
Range<T> operator*(const Range<T>& a, T v)
{
    return Range(a) *= v;
}
template<typename T>
Range<T> operator/(const Range<T>& a, T v)
{
    return Range(a) /= v;
}
template<typename T>
Range<T> operator&(const Range<T>& a, const Range<T>& b)
{
    auto begin = std::max(a.begin, b.begin);
    return Range(begin, std::max(begin, std::min(a.end, b.end)));
}

template<typename T>
std::ostream& operator<<(std::ostream& s, const Range<T>& range)
{
    s << "[" << range.begin << " ... " << range.end << "]";
    return s;
}

//TODO, see if ... is a good idea template<typename T> std::istream& operator>>(std::istream& s, Range<T>& range)
}

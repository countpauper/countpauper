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
    Range() = default;
    Range(T begin, T end) :
        begin(begin),
        end(end)
    {
    }

    template<typename OT>
    explicit Range(Range<OT> o) :
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
        return (begin <= v) && (v+ElementSize() <= end);
    }

    Range& operator|=(T v)
    {
        begin = std::min(begin, v);
        end = std::max(end, v+ElementSize());
        return *this;
    }


    Range& operator+=(T v)
    {
        begin += v;
        end += v;
        return *this;
    }
    Range& operator-=(T v) { return operator+=(-v); }
    Range& operator*=(T v)
    {
        begin *= v;
        end *= v;
        return *this;
    }
    Range& operator/=(T v)
    {
        return operator*=(1 / v);
    }

    Range& operator|=(const Range<T>& o)
    {
        *this = *this | o;
        return *this;
    }

    Range& operator&=(const Range<T>& o)
    {
        *this = *this & o;
        return *this;
    }

    bool operator<(T v) const
    {
        return end <= v;
    }
    bool operator<=(T v) const
    {
        return begin >= v;
    }
    bool operator>(T v) const
    {
        return begin > v;
    }
    bool operator>=(T v) const
    {
        return end > v;
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
    T Clip(T v) const
    {
        return Engine::Clip(v, begin, end - ElementSize());
    }

    T Middle() const
    {
        return Middle(std::is_floating_point<T>());
    }

    T Middle(std::true_type) const
    {
        if ((std::isinf(end)) && (std::isinf(begin)) && (std::signbit(begin)!=std::signbit(end)))
            return 0;
        else
            return (begin + end) / 2;
    }

    T Middle(std::false_type) const
    {
        return (begin + end) / 2;
    }


    T begin;
    T end;


    bool infinite() const
    {
        return std::isinf(begin) || std::isinf(end);
    }

    bool finite() const
    {
        return !infinite();
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

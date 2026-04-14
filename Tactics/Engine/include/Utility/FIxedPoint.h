#pragma once
#include <cmath>

namespace Engine
{

template<unsigned bits=16, std::integral T=int>
class FixedPoint
{
public:
    using FPT = FixedPoint<bits, T>;

    FixedPoint() = default;
    constexpr FixedPoint(float input) :
        v(std::round(input * FractionDivisor()))
    {
        static_assert(bits < std::log2(std::numeric_limits<T>::max()));

        if (input * FractionDivisor() > std::numeric_limits<T>::max())
            throw std::range_error("Floating point is too large for fixed point");
        if (input * FractionDivisor() < std::numeric_limits<T>::lowest())
            throw std::range_error("Floating point is too small for fixed point");
    }
    constexpr FixedPoint(double input) :
        v(std::round(input * FractionDivisor()))
    {
        if (input * FractionDivisor() > std::numeric_limits<T>::max())
            throw std::range_error("Double precision floating point is too large for fixed point");
        if (input * FractionDivisor() < std::numeric_limits<T>::lowest())
            throw std::range_error("Double precision floating point is too small for fixed point");
    }

    constexpr FixedPoint(T input) :
        v(input * FractionDivisor())
    {
        if (input>std::numeric_limits<T>::max() >> bits)
            throw std::range_error("Integer is too large for fixed point");
        if (input < std::numeric_limits<T>::lowest() >> bits)
            throw std::range_error("Integer is too small for fixed point");
    }

    template<unsigned OB, typename OT>
    constexpr FixedPoint(FixedPoint<OB, OT> o)
    {
        if (OB > bits)
            v = static_cast<T>(o.v >> (OB - bits));
        else
            v = static_cast<T>(o.v << (bits - OB));
    }

    FixedPoint(const FPT& o) :
        v(o.v)
    {
    }

    FPT& operator=(const FPT& o)
    {
        v = o.v;
        return *this;
    }

    explicit operator float() const
    {
        return Integral() + float(FracionBits()) / FractionDivisor();
    }

    explicit operator double() const
    {
        return Integral() + double(FracionBits()) / FractionDivisor();
    }

    explicit operator T() const
    {
        return Integral();
    }



    FPT& operator+=(FPT rhs)
    {
        v += rhs.v;
        return *this;
    }

    FPT& operator-=(FPT rhs)
    {
        v -= rhs.v;
        return *this;
    }

    FPT& operator*=(FPT rhs)
    {
        assert(sizeof(T) <= sizeof(uint64_t)/2);  // requires a type with double the size of the original for intermediate result. For now 64 bits is used
        if (std::is_signed<T>())
            v = static_cast<T>((static_cast<int64_t>(v)*rhs.v) >> bits);
        else
            v = static_cast<T>((static_cast<uint64_t>(v)*rhs.v) >> bits);
        return *this;
    }

    FPT& operator/=(FPT rhs)
    {
        assert(sizeof(T) <= sizeof(uint64_t)/2);  // requires a type with double the size of the original  for intermediate result. For now 64 bits is used.
        if (std::is_signed<T>())
            v = static_cast<T>((static_cast<int64_t>(v) << bits) / rhs.v);
        else
            v = static_cast<T>((static_cast<uint64_t>(v) << bits) / rhs.v);
        return *this;
    }

    bool operator==(FPT rhs) const
    {
        return v == rhs.v;
    }

    bool operator!=(FPT rhs) const
    {
        return v != rhs.v;
    }

    bool operator<(FPT rhs) const
    {
        return v < rhs.v;
    }

    bool operator<=(FPT rhs) const
    {
        return v <= rhs.v;
    }

    bool operator>(FPT rhs) const
    {
        return v > rhs.v;
    }

    bool operator>=(FPT rhs) const
    {
        return v >= rhs.v;
    }


private:
    constexpr T Integral() const
    {
        return v>>bits;
    }
    constexpr T FracionBits() const
    {
        return v & FractionMask();
    }
    static constexpr uint64_t FractionDivisor()
    {
        return 1ULL<<bits;
    }
    static constexpr uint64_t FractionMask()
    {
        return FractionDivisor()-1;
    }

    template<unsigned OB, std::integral OT>
    friend class FixedPoint;

    T v;
};

template<unsigned bits, typename T>
FixedPoint<bits, T> operator+(FixedPoint<bits,T> lhs, FixedPoint<bits, T> rhs)
{
    FixedPoint<bits, T> result(lhs);
    return lhs += rhs;
}

template<unsigned bits, typename T>
FixedPoint<bits, T> operator-(FixedPoint<bits,T> lhs, FixedPoint<bits, T> rhs)
{
    FixedPoint<bits, T> result(lhs);
    return lhs -= rhs;
}

template<unsigned bits, typename T>
FixedPoint<bits, T> operator*(FixedPoint<bits,T> lhs, FixedPoint<bits, T> rhs)
{
    FixedPoint<bits, T> result(lhs);
    return lhs *= rhs;
}

template<unsigned bits, typename T>
FixedPoint<bits, T> operator/(FixedPoint<bits,T> lhs, FixedPoint<bits, T> rhs)
{
    FixedPoint<bits, T> result(lhs);
    return lhs /= rhs;
}

template<unsigned bits=16, std::integral T=int>
std::ostream& operator<<(std::ostream& stream, FixedPoint<bits, T> value)
{
    stream << static_cast<double>(value);
    return stream;
}

}


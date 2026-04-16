#pragma once
#include <limits>
#include <cmath>
#include <cassert>

namespace Engine
{

template<unsigned bits=16, std::integral T=int>
class FixedPoint
{
public:
    using FPT = FixedPoint<bits, T>;
    using IntegerType = T;

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
        return Integral() + float(FractionBits()) / FractionDivisor();
    }

    explicit operator double() const
    {
        return Integral() + double(FractionBits()) / FractionDivisor();
    }

    explicit operator T() const
    {
        return Integral();
    }

    constexpr T RawValue() const
    {
        return v;
    }

    FPT operator-() const
    {
        return FromRaw(-v);
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

    template<std::floating_point RHT>
    FPT& operator*=(RHT rhs)
    {
        return *this = FPT(static_cast<RHT>(*this) * rhs);
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

    template<std::floating_point RHT>
    FPT& operator/=(RHT rhs)
    {
        return *this = FPT(static_cast<RHT>(*this) / rhs);
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
    template<unsigned OB, std::integral OT> friend FixedPoint<OB,OT> abs(FixedPoint<OB,OT> fp);
    template<unsigned OB, std::integral OT> friend FixedPoint<OB,OT> modfp(FixedPoint<OB,OT>, OT&);
    template<unsigned OB, std::integral OT> friend bool signbit(FixedPoint<OB, OT> fp);
    template<unsigned OB, std::integral OT> friend OT round(FixedPoint<OB, OT> fp);
    template<unsigned OB, std::integral OT> friend OT ceil(FixedPoint<OB, OT> fp);
    template<unsigned OB, std::integral OT> friend OT floor(FixedPoint<OB, OT> fp);
    template<typename _T> friend struct std::numeric_limits;

    constexpr T Integral() const
    {
        return v>>bits;
    }
    constexpr T FractionBits() const
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

    static FPT FromRaw(T v)
    {
        FPT result;
        result.v = v;
        return result;
    }

    template<unsigned OB, std::integral OT>
    friend class FixedPoint;

    T v;
};

template<unsigned bits, typename T, typename RHT>
FixedPoint<bits, T> operator+(FixedPoint<bits,T> lhs, RHT rhs)
{
    FixedPoint<bits, T> result(lhs);
    return lhs += rhs;
}

template<unsigned bits, typename T, typename RHT>
FixedPoint<bits, T> operator-(FixedPoint<bits,T> lhs, RHT rhs)
{
    FixedPoint<bits, T> result(lhs);
    return lhs -= rhs;
}

template<unsigned bits, typename T, typename RHT>
FixedPoint<bits, T> operator*(FixedPoint<bits,T> lhs, RHT rhs)
{
    FixedPoint<bits, T> result(lhs);
    return lhs *= rhs;
}

template<unsigned bits, typename T, typename RHT>
FixedPoint<bits, T> operator/(FixedPoint<bits,T> lhs, RHT rhs)
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

template<unsigned B, std::integral T>
FixedPoint<B,T> abs(FixedPoint<B,T> fp)
{
    return FixedPoint<B,T>::FromRaw(std::abs(fp.RawValue()));
}

template<unsigned B, std::integral T>
FixedPoint<B,T> modfp(FixedPoint<B,T> fp, T& integer)
{
    if (signbit(fp))
    {
        fp = -fp;
        integer = -(fp.Integral());
        return -FixedPoint<B,T>::FromRaw(fp.FractionBits());
    }
    else
    {
        integer = fp.Integral();
        return FixedPoint<B,T>::FromRaw(fp.FractionBits());
    }
}

template<unsigned B, std::integral T>
bool signbit(FixedPoint<B,T> fp)
{
    return std::signbit(fp.RawValue());
}

template<unsigned B, std::integral T>
T round(FixedPoint<B,T> fp)
{
    if constexpr (B==0)
        return fp.Integral();
    else
    {
        fp -= FixedPoint<B,T>::FromRaw(signbit(fp));
        T Int = fp.Integral();
        T fraction = fp.FractionBits();
        T roundInt = fraction >> (B-1);
        return Int + roundInt;
    }
}

template<unsigned B, std::integral T>
T floor(FixedPoint<B,T> fp)
{
    return fp.Integral();
}

template<unsigned B, std::integral T>
T ceil(FixedPoint<B,T> fp)
{
    return fp.Integral() + (fp.FractionBits()!=0);
}


template<unsigned B, std::integral T>
constexpr FixedPoint<B,T> lerp(FixedPoint<B,T> __a, FixedPoint<B,T> __b, float __t) noexcept
{
    return FixedPoint<B,T>(std::__lerp(static_cast<float>(__a), static_cast<float>(__b), __t));
}

}

namespace std
{ // Risky to add custom things to std namespace, but then numeric_limits is already in it and it is used otherwise generically.

template<unsigned B, integral T>
struct numeric_limits<Engine::FixedPoint<B,T>>
{
public:
    using FPT = Engine::FixedPoint<B,T>;

    static constexpr bool is_specialized = true;
    static constexpr bool is_signed = numeric_limits<T>::is_signed;
    static constexpr bool is_integer = B==0;
    static constexpr bool is_exact = false;
    static constexpr bool has_quiet_NaN = numeric_limits<T>::has_quiet_NaN;
    static constexpr bool has_signaling_NaN = numeric_limits<T>::has_signaling_NaN;
    static constexpr float_denorm_style has_denorm = denorm_absent;
    static constexpr bool has_denorm_loss = false;
    static constexpr float_round_style round_style = round_to_nearest;
    static constexpr bool is_iec559 = false;
    static constexpr bool is_bounded = numeric_limits<T>::is_bounded;
    static constexpr bool is_modulo = numeric_limits<T>::is_modulo;
    static constexpr int digits = numeric_limits<T>::digits - B;
    static constexpr int digits10 = numeric_limits<FPT>::digits * log10(2);
    static constexpr int radix = numeric_limits<T>::radix;
    static constexpr int min_exponent = 0;
    static constexpr int min_exponent10 = 0;
    static constexpr int max_exponent = 0;
    static constexpr int max_exponent10 = 0;
    static constexpr bool traps = numeric_limits<T>::traps;
    static constexpr bool tinyness_before = false;

    static constexpr FPT max()
    {
        return FPT::FromRaw(numeric_limits<T>::max());
    }

    static constexpr FPT min()
    {
        return FPT::FromRaw(numeric_limits<T>::min());
    }

    static constexpr FPT denorm_min()
    {
        return FPT::FromRaw(numeric_limits<T>::min());
    }

    static constexpr FPT lowest()
    {
        return FPT::FromRaw(numeric_limits<T>::lowest());
    }

    static constexpr FPT epsilon()
    {
        return FPT::FromRaw(1);
    }

    static constexpr FPT round_error()
    {
        if (B==0)
            return FPT::FromRaw(1);
        else
            return FPT::FromRaw(1 << (B-1));
    }

    static constexpr FPT infinity()
    {
        return FPT::FromRaw(0);
    }

    static constexpr FPT quiet_NaN()
    {
        return FPT::FromRaw(0);
    }

    static constexpr FPT  signaling_NaN()
    {
        return FPT::FromRaw(0);
    }


};

}

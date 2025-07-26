#include "Logic/Internal/Number.h"
#include "Logic/Expression.h"
#include <sstream>
#include <cmath>

namespace Angel::Logic
{

Number Make(double real, double imaginary)
{
    assert(imaginary == 0); // not implemented;
    if (std::isnan(real))
        return Real(real);
    if (real > std::numeric_limits<long>::max())
        return Real(real);
    if (real < std::numeric_limits<long>::min())
        return Real(real);
    double intpart;
    if (modf(real, &intpart) == 0)
        return Integer(intpart);
    else
        return Real(real);
}

Number::Number(double real, double imaginary) : 
    Number(Make(real, imaginary))
{
}

Number& Number::operator=(const Number& e)
{
    NumberVariant::operator=(e);
    return *this;
}

Number& Number::operator=(Number&& e)
{
    NumberVariant::operator=(std::move(e));
    return *this;
}

bool Number::operator==(const Number& rhs) const
{
    return std::visit(
        [this](const auto& rho)
        {
            return operator==(rho);
        }, rhs);
}

Number& Number::operator+=(const Number& rhs) 
{ 
     return operator=(std::visit(overloaded_visit{
        [&rhs](Boolean& obj) -> Number
        {
            return Integer(obj) + rhs.Cast<Integer>();
        },
        [&rhs](auto& obj) -> Number
        {
            return obj + rhs.Cast<std::remove_reference_t<decltype(obj)>>();
        }
    }, *this));
}

Number& Number::operator-=(const Number& rhs) 
{ 
     return operator=(std::visit(overloaded_visit{
        [&rhs](Boolean& obj) -> Number
        {
            return Integer(obj) - rhs.Cast<Integer>();
        },
        [&rhs](auto& obj) -> Number
        {
            return obj - rhs.Cast<std::remove_reference_t<decltype(obj)>>();
        }
    }, *this));
}

Number& Number::operator*=(const Number& rhs) 
{ 
     return operator=(std::visit(overloaded_visit{
        [&rhs](Boolean& obj) -> Number
        {
            return Integer(obj) * rhs.Cast<Integer>();
        },
        [&rhs](auto& obj) -> Number
        {
            return obj * rhs.Cast<std::remove_reference_t<decltype(obj)>>();
        }
    }, *this));
    return *this;
}

Number& Number::operator/=(const Number& rhs) 
{ 
     return operator=(std::visit(overloaded_visit{
        [&rhs](Boolean& obj) -> Number
        {
            return Integer(obj) / rhs.Cast<Integer>();
        },
        [&rhs](auto& obj) -> Number
        {
            return obj / rhs.Cast<std::remove_reference_t<decltype(obj)>>();
        }
    }, *this));
}

Number& Number::operator^=(const Number& rhs)
{ 
     return operator=(std::visit(overloaded_visit{
        [&rhs](Boolean& obj) -> Number
        {
            return Integer(obj) ^ rhs.Cast<Integer>();
        },
        [&rhs](auto& obj) -> Number
        {
            return obj ^ rhs.Cast<std::remove_reference_t<decltype(obj)>>();
        }
    }, *this));
}

Number& Number::logis(const Number& rhs)
{
     return operator=(std::visit(overloaded_visit{
        [&rhs](const Boolean& obj) -> Number
        {
            return Logic::log(Integer(obj), rhs.Cast<Integer>());
        },
        [&rhs](const auto& obj) -> Number
        {
            return Logic::log(obj, rhs.Cast<std::remove_reference_t<decltype(obj)>>());
        }
    }, *this));
}

Number Number::operator-() const
{
     return std::visit(overloaded_visit{
        [](const Boolean& obj) -> Number
        {
            return -Integer(obj);
        },
        [](const auto& obj) -> Number
        {
            return -obj;
        }
    }, *this);
}



bool Number::operator<(const Number& o) const
{
    return Hash() < o.Hash();
}

std::string to_string(const Number& c)
{
    std::stringstream ss;
    ss << c;
    return ss.str();
}

Number operator+(Number lhs, const Number& rhs) { return lhs+= rhs; }
Number operator-(Number lhs, const Number& rhs) { return lhs-= rhs; }
Number operator*(Number lhs, const Number& rhs) { return lhs*= rhs; }
Number operator/(Number lhs, const Number& rhs) { return lhs/= rhs; }
Number operator^(Number lhs, const Number& rhs) { return lhs^= rhs; }

std::ostream& operator<<(std::ostream& s, const Number& c)
{
    std::visit(
        [&s](const auto& obj)
        {
            s << obj;
        }, c);
    return s;
}

}
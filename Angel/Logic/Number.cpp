#include "Logic/Internal/Number.h"
#include "Logic/Expression.h"
#include <sstream>

namespace Angel::Logic
{

Number& Number::operator=(const Number& e)
{
    NumberVariant::operator=(e);
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

Expression Number::operator+=(const Number& rhs) 
{ 
     return std::visit(overloaded_visit{
        [&rhs](Boolean& obj) -> Expression
        {
            return Integer(obj) += rhs.Cast<Integer>();
        },
        [&rhs](auto& obj) -> Expression
        {
            return obj += rhs.Cast<Integer>();
        }
    }, *this);
}

Expression Number::operator-=(const Number& rhs) 
{ 
     return std::visit(overloaded_visit{
        [&rhs](Boolean& obj) -> Expression
        {
            return Integer(obj) -= rhs.Cast<Integer>();
        },
        [&rhs](auto& obj) -> Expression
        {
            return obj -= rhs.Cast<Integer>();
        }
    }, *this);
}

Expression Number::operator*=(const Number& rhs) 
{ 
     return std::visit(overloaded_visit{
        [&rhs](Boolean& obj) -> Expression
        {
            return Integer(obj) *= rhs.Cast<Integer>();
        },
        [&rhs](auto& obj) -> Expression
        {
            return obj *= rhs.Cast<Integer>();
        }
    }, *this);
}

Expression Number::operator/=(const Number& rhs) 
{ 
     return std::visit(overloaded_visit{
        [&rhs](Boolean& obj) -> Expression
        {
            return Integer(obj) /= rhs.Cast<Integer>();
        },
        [&rhs](auto& obj) -> Expression
        {
            return obj /= rhs.Cast<Integer>();
        }
    }, *this);
}

Expression Number::operator^=(const Number& rhs) 
{ 
     return std::visit(overloaded_visit{
        [&rhs](Boolean& obj) -> Expression
        {
            return Integer(obj) ^= rhs.Cast<Integer>();
        },
        [&rhs](auto& obj) -> Expression
        {
            return obj ^= rhs.Cast<Integer>();
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


Expression operator+(Number lhs, const Number& rhs) { return lhs+= rhs; }
Expression operator-(Number lhs, const Number& rhs) { return lhs-= rhs; }
Expression operator*(Number lhs, const Number& rhs) { return lhs*= rhs; }
Expression operator/(Number lhs, const Number& rhs) { return lhs/= rhs; }
Expression operator^(Number lhs, const Number& rhs) { return lhs^= rhs; }

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
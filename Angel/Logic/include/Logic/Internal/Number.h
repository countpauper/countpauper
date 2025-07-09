#pragma once

#include "Logic/Integer.h"
//#include "Logic/Float.h"
//#include "Logic/Complex.h"
#include "Logic/Boolean.h"
#include "Logic/Internal/Variant.h"


namespace Angel::Logic
{

using NumberVariant = Variant<
    Integer, 
//    Float, 
//    Complex,
    Boolean>;
class const_container_iterator;

class Number : public NumberVariant
{
public:
    Number(const Number&) = default;
    using NumberVariant::NumberVariant;
    Number& operator=(const Number& e);

    template<typename T> 
    requires is_alternative<T, NumberVariant>
    bool operator==(const T& rhs) const
    {
        return std::visit(overloaded_visit{
            [&rhs](const T& lv)
            {
                return lv == rhs;
            }, 
            [](const auto&) { return false; }   
            },*this);
    }
    bool operator==(const Number& rhs) const;
 
    template<typename T> 
    bool operator<(const T& rhs) const
    {
        return std::visit(
            [this, &rhs](const auto& obj) 
            {   // TODO: could even generalize and iterate here? 
                std::hash<Expression> hasher;
                return this->Hash() < hasher(rhs);
            },*this);
    }
    bool operator<(const Number& o) const;
    inline bool operator<=(const Number& o) const { return *this==o || *this<o; }
    inline bool operator>(const Number& o) const { return !(*this<=o); }
    inline bool operator>=(const Number& o) const { return !(*this<o); }
    Expression operator+=(const Number& rhs);
    Expression operator-=(const Number& rhs);    
    Expression operator*=(const Number& rhs);
    Expression operator/=(const Number& rhs);    
    Expression operator^=(const Number& rhs); 
    std::string Summary() const;
};

Expression operator+(Number lhs, const Number& rhs);
Expression operator-(Number lhs, const Number& rhs);
Expression operator*(Number lhs, const Number& rhs);
Expression operator/(Number lhs, const Number& rhs);
Expression operator^(Number lhs, const Number& rhs);
std::ostream& operator<<(std::ostream& s, const Number& e);
}

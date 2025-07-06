#pragma once

#include "Logic/Set.h"
#include "Logic/List.h"
#include "Logic/All.h"
#include "Internal/Variant.h"
#include <variant>


namespace Angel::Logic
{

using ContainerVariant = Variant<
    List, Set, All>;  
class const_container_iterator;

class Container : public ContainerVariant
{
public:
    Container(const Container&) = default;
    using ContainerVariant::ContainerVariant;
    
    Container& operator=(const Container& e);

    std::size_t size() const;

    Container Simplify() const;
    Set Assumptions() const;
    inline bool IsConstant() const { return Assumptions().empty(); }
    Container Substitute(const Hypothesis& hypothesis) const;
    Expression Matches(const Expression& e, const Hypothesis& hypothesis) const;
    Expression Infer(const class Knowledge& knowledge, const Hypothesis& hypothesis, Trace& trace) const;

    explicit operator bool() const;

    template<typename T> 
    requires is_alternative<T, ContainerVariant>
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
    bool operator==(const Container& rhs) const;
 
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
    const_container_iterator begin() const;
    const_container_iterator end() const;
    const_container_iterator find(const Expression& item) const;
    const_container_iterator erase(const_container_iterator item);

    bool operator<(const Container& o) const;
    inline bool operator<=(const Container& o) const { return *this==o || *this<o; }
    inline bool operator>(const Container& o) const { return !(*this<=o); }
    inline bool operator>=(const Container& o) const { return !(*this<o); }
    Expression operator+=(const Container& rhs);
    Expression operator-=(const Container& rhs);    
    Expression operator&=(const Container& rhs);
    Expression operator|=(const Container& rhs);    
    std::string Summary() const;
};

Expression operator+(Container lhs, const Container& rhs);
Expression operator-(Container lhs, const Container& rhs);
Expression operator&(Container lhs, const Container& rhs);
Expression operator|(Container lhs, const Container& rhs);
std::ostream& operator<<(std::ostream& s, const Container& e);
}

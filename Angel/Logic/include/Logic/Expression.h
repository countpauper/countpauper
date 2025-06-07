#pragma once

#include "Logic/Element.h"
#include "Logic/Predicate.h"
#include "Logic/Set.h"
#include "Logic/List.h"
#include "Logic/Negative.h"
#include "Logic/Conjunction.h"
#include "Logic/Disjunction.h"
#include "Logic/Summation.h"
#include "Logic/Subtraction.h"
#include "Logic/Equation.h"
#include "Logic/Ordering.h"
#include "Logic/Function.h"
#include "Logic/Association.h"
#include "Logic/Variable.h"
#include "Logic/Tuple.h"
#include "Logic/Operator.h"
#include "Logic/CastException.h"
#include "Logic/Internal/VariantUtils.h"
#include <variant>
#include <vector>
#include <map>
#include <type_traits>

namespace std
{
	template <>
	struct hash<Angel::Logic::Expression>
	{
		size_t operator()(const Angel::Logic::Expression& n) const;
	};
}

namespace Angel::Logic
{

using ExpressionVariant = std::variant<
    std::monostate,
    Function,      
    Boolean,  Integer, Id, String,
    Variable, Tuple, 
    Predicate, List, Set, Association,
    Negative, 
    Conjunction, Disjunction, 
    Summation, Subtraction,
    Equation, Lesser, LesserEqual, Greater, GreaterEqual>;  

template <typename T, typename O>
concept is_ordered = requires(const T& a, const O& b) {
    { a < b } -> std::convertible_to<bool>;
};

class Expression : public ExpressionVariant
{
public:
    Expression() = default;
    
    template<typename T>
    requires is_alternative<T, ExpressionVariant>
    Expression(const T& v) :
        ExpressionVariant(v)
    {
    }
    Expression(const Expression& e);
    Expression(const Operator ope, Collection&& operands);
    Expression& operator=(const Expression& e);

    template<typename T>
    bool Is() const
    {
        return std::get_if<T>(this) != nullptr;
    }

    template<typename T>
    const T* GetIf() const 
    {
        return std::get_if<T>(this);
    }

    template<typename T> 
    const T& Get() const 
    {
        return std::get<T>(*this);
    }
    template<typename T>
    const std::optional<T> TryCast() const
    {
        auto same = std::get_if<T>(this);
        if (same)
            return std::optional<T>(*same);
        else
        {
            return std::visit(overloaded_visit{
                // Automatically cast any alternative that has a constructor that 
                // takes another alternative const& as its sole argument
                [this](const auto& castee) 
                requires std::is_constructible_v<T, decltype(castee)>
                {
                    return std::optional<T>(castee);
                },
                [](const auto&) 
                {
                    return std::optional<T>();
                }
            }, *this);
        }
    }


    template<typename T>
    const T Cast() const
    {
        auto maybe = TryCast<T>();
        if (maybe)
            return *maybe;
        throw CastException(AlternativeTypeId(), typeid(T));
    }
    Expression TryCast(const std::type_info& rtt) const;
    Expression Cast(const std::type_info& rtt) const;

    Expression Simplify() const;
    std::size_t Assumptions() const;
    inline bool IsConstant() const { return Assumptions() == 0; }
    Expression Substitute(const Hypothesis& hypothesis) const;
    Expression Matches(const Expression& e, const Hypothesis& hypothesis) const;
    Expression Infer(const class Knowledge& knowledge, const Hypothesis& hypothesis, Trace& trace) const;

    explicit operator bool() const;

    template<typename T> 
    requires(!std::is_same_v<Expression, T>) 
    bool operator==(const T& rhs) const
    {
        return std::visit(overloaded_visit{
            [](std::monostate)
            {
                return false;
            },
            [&rhs](const T& lv)
            {
                return lv == rhs;
            }, 
            [](const auto&) { return false; }   
            },*this);
    }
    bool operator==(const Expression& rhs) const;
 
    template<typename T> 
    bool operator<(const T& rhs) const
    {
        return std::visit(overloaded_visit{
            [](std::monostate)
            {
                return false;
            },
            [&rhs](const auto& lhs)
            requires is_ordered<decltype(lhs), T>
            {
                return lhs < rhs; 
            }, 
            [this, &rhs](const auto& obj) 
            { 
                std::hash<Expression> hasher;
                return hasher(*this) < hasher(rhs);
            }   
            },*this);
    }

    bool operator<(const Expression& o) const;
    inline bool operator<=(const Expression& o) const { return *this==o || *this<o; }
    inline bool operator>(const Expression& o) const { return !(*this>=o); }
    inline bool operator>=(const Expression& o) const { return !(*this<o); }

    const std::type_info& AlternativeTypeId() const;

    std::string Summary() const;
    std::string Documentation() const;

};

std::ostream& operator<<(std::ostream& s, const Expression& e);
std::string to_string(const Expression& e);

}



// Template implementations that depend on Expression and are therefore forward declared
#include "Logic/Internal/IntOperation.h"
#include "Logic/Internal/IntOperationWithBase.h"
#include "Logic/Internal/IntComparison.h"
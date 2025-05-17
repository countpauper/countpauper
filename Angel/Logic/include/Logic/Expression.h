#pragma once

#include "Logic/Element.h"
#include "Logic/Predicate.h"
#include "Logic/Set.h"
#include "Logic/List.h"
#include "Logic/CastException.h"
#include "Logic/Conjunction.h"
#include "Logic/Disjunction.h"
#include "Logic/Summation.h"
#include "Logic/VariantUtils.h"
#include "Logic/Operator.h"
#include "Logic/Negative.h"
#include <variant>
#include <vector>
#include <map>
#include <type_traits>

namespace Angel::Logic
{
class Object;

using ExpressionVariant = std::variant<Boolean,  Integer, Id, Variable, Predicate, 
    List, Set, 
    Negative,
    Conjunction, Disjunction, Summation>; 

class Expression : public ExpressionVariant
{
public:
    template<typename T>
    requires is_alternative<T, ExpressionVariant>
    Expression(const T& v) :
        ExpressionVariant(v)
    {
    }
    Expression(const Expression& e);
    Expression(const Operator ope, Collection&& operands);

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
                [](const auto& obj) 
                {
                    return std::optional<T>();
                }
            }, *this);
        }
    }
    const std::type_info& AlternativeTypeInfo() const 
    {
        return *std::visit([](const auto& obj)
        {
            return &typeid(decltype(obj)); 
        }, *this);        
    }

    template<typename T>
    const T Cast() const
    {
        auto maybe = TryCast<T>();
        if (maybe)
            return *maybe;
        throw CastException(AlternativeTypeInfo(), typeid(T));

    }
    Match Matches(const Expression& e, const Variables& substitutions) const;
    Object Infer(const class Knowledge& knowledge, const Variables& substitutions) const;

    template<typename T> 
    requires(!std::is_same_v<Expression, T>) 
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

    bool operator<(const Expression&o) const;
};

std::ostream& operator<<(std::ostream& s, const Expression& e);

std::string to_string(const Expression& e);
}

namespace std
{
	template <>
	struct hash<Angel::Logic::Expression>
	{
		size_t operator()(const Angel::Logic::Expression& n) const;
	};
}
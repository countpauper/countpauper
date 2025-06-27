#pragma once

#include "Logic/Element.h"
#include "Logic/Predicate.h"
#include "Logic/Container.h"
#include "Logic/Negative.h"
#include "Logic/Conjunction.h"
#include "Logic/Disjunction.h"
#include "Logic/Summation.h"
#include "Logic/Subtraction.h"
#include "Logic/Multiplication.h"
#include "Logic/Division.h"
#include "Logic/Ordering.h"
#include "Logic/Function.h"
#include "Logic/Association.h"
#include "Logic/Variable.h"
#include "Logic/Operator.h"
#include "Logic/CastException.h"
#include "Logic/Internal/Variant.h"
#include "Logic/Hash.h"
#include <variant>

namespace Angel::Logic
{

using ExpressionVariant = Variant<
    std::monostate,
    Function,      
    Boolean,  Integer, Id, String,
    Variable, Container,
    Predicate, Association,
    Negative, 
    Conjunction, Disjunction, 
    Summation, Subtraction, Multiplication, Division,
    Equal, Unequal, Lesser, LesserEqual, Greater, GreaterEqual>;  

template <typename T, typename O>
concept is_ordered = requires(const T& a, const O& b) {
    { a < b } -> std::convertible_to<bool>;
};

class Expression : public ExpressionVariant
{
public:
    Expression() = default;
    
    using ExpressionVariant::ExpressionVariant;

    template<typename T>
    requires is_alternative<T, ContainerVariant>
    Expression(const T& v) :
        ExpressionVariant(Container(v))
    {
    }    
    Expression(const Expression& e);
    Expression(const Operator ope, Tuple&& operands);
    Expression& operator=(const Expression& e);

    template<class CT>
    requires is_alternative<CT, Container>
    bool Is() const
    {
        auto container = GetIf<Container>();
        return (container) && container->Is<CT>();
    }
    using ExpressionVariant::Is;

    template<typename CT>
    requires is_alternative<CT, ContainerVariant>
    const CT* GetIf() const 
    {
        auto container = GetIf<Container>();
        if (!container)
            return nullptr;
        return container->GetIf<CT>();
    }
    template<typename CT>
    requires is_alternative<CT, ContainerVariant>
    CT* GetIf() 
    {
        auto container = GetIf<Container>();
        if (!container)
            return nullptr;
        return container->GetIf<CT>();
    }

    using ExpressionVariant::GetIf;

    template<typename CT>
    requires is_alternative<CT, ContainerVariant>
    const CT& Get() const 
    {
        return Get<Container>().Get<CT>();
    }

    using ExpressionVariant::Get;

    template<typename T>
    const std::optional<T> TryCast() const
    {
        if (const auto* container = GetIf<Container>())
            return container->TryCast<T>();
        else
            return ExpressionVariant::TryCast<T>();
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
    Operator GetOperator() const;
    bool IsComparison() const;
    std::size_t size() const;   // 0 for non containers and non operations

    Expression Simplify() const;
    Set Assumptions() const;
    inline bool IsConstant() const { return Assumptions().empty(); }
    Expression Substitute(const Hypothesis& hypothesis) const;
    Expression Matches(const Expression& e, const Hypothesis& hypothesis) const;
    Expression Infer(const class Knowledge& knowledge, const Hypothesis& hypothesis, Trace& trace) const;

    explicit operator bool() const;

    template<typename CT> 
    requires is_alternative<CT, ContainerVariant>
    bool operator==(const CT& rhs) const
    {
        return std::visit(overloaded_visit{
            [&rhs](const Container& container)
            {
                return container == rhs;
            }, 
            [](const auto&) { return false; }   
        }, *this);
    }
    using ExpressionVariant::operator==;

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
    inline bool operator>(const Expression& o) const { return !(*this<=o); }
    inline bool operator>=(const Expression& o) const { return !(*this<o); }

    std::string Summary() const;
    std::string Documentation() const;

};




std::ostream& operator<<(std::ostream& s, const Expression& e);
std::string to_string(const Expression& e);

}

// Template implementations that depend on Expression and are therefore forward declared
#include "Logic/Internal/ExpressionImpl.h"
#include "Logic/Internal/OperationImpl.h"
#include "Logic/Internal/FlatCollectionImpl.h"
#include "Logic/Internal/OperationWithBaseImpl.h"
#include "Logic/Internal/ComparisonImpl.h"

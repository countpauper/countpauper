#pragma once

#include "Logic/Element.h"
#include "Logic/Predicate.h"
#include "Logic/Negative.h"
#include "Logic/Internal/Container.h"
#include "Logic/Internal/Number.h"
#include "Logic/Internal/Ordering.h"
#include "Logic/Internal/Operation.h"
#include "Logic/Function.h"
#include "Logic/Association.h"
#include "Logic/Variable.h"
#include "Logic/CastException.h"
#include "Logic/Internal/Variant.h"
#include "Logic/Internal/Hash.h"
#include <variant>

namespace Angel::Logic
{

using ExpressionVariant = Variant<
    std::monostate,
    Function,      
    Number,
    Id, String, 
    Container,
    Predicate, Association,
    Operation, 
    Ordering>;  

template <typename T, typename O>
concept is_ordered = requires(const T& a, const O& b) {
    { a < b } -> std::convertible_to<bool>;
};

class Expression : 
    public ExpressionVariant
{
public:
    Expression() = default;
    
    using ExpressionVariant::ExpressionVariant;
    template<is_alternative<Container> T>
    Expression(const T& v) : Expression(Container(v)) {} 
    template<is_alternative<Operation> T>
    Expression(const T& v) : Expression(Operation(v)) {} 
    template<is_alternative<Ordering> T>
    Expression(const T& v) : Expression(Ordering(v)) {} 
    template<is_alternative<Number> T>
    Expression(const T& v) : Expression(Number(v)) {} 

    Expression(const Expression& e);
    Expression(const Operator ope, Tuple&& operands);
    Expression& operator=(const Expression& e);

    using ExpressionVariant::Is;
    using ExpressionVariant::GetIf;
    using ExpressionVariant::Get;

    template<is_alternative<Number> CT>
    bool Is() const
    {
        auto sub = GetIf<Number>();
        return (sub) && sub->template Is<CT>();
    }

    template<is_alternative<Container> CT>
    bool Is() const
    {
        auto sub = GetIf<Container>();
        return (sub) && sub->template Is<CT>();
    }
    
    template<is_alternative<Operation> CT>
    bool Is() const
    {
        auto sub = GetIf<Operation>();
        return (sub) && sub->template Is<CT>();
    }
    
    template<is_alternative<Ordering> CT>
    bool Is() const
    {
        auto sub = GetIf<Ordering>();
        return (sub) && sub->template Is<CT>();
    }

    template<is_alternative<Number> T>
    const T* GetIf() const 
    {
        auto sub = GetIf<Number>();
        if (!sub)
            return nullptr;
        return sub->template GetIf<T>();
    }

    template<is_alternative<Number> T>
    T* GetIf() 
    {
        auto sub = GetIf<Number>();
        if (!sub)
            return nullptr;
        return sub->template GetIf<T>();
    }

    template<is_alternative<Container> T>
    const T* GetIf() const 
    {
        auto sub = GetIf<Container>();
        if (!sub)
            return nullptr;
        return sub->template GetIf<T>();
    }

    template<is_alternative<Container> T>
    T* GetIf() 
    {
        auto sub = GetIf<Container>();
        if (!sub)
            return nullptr;
        return sub->template GetIf<T>();
    }

    template<is_alternative<Operation> T>
    const T* GetIf() const 
    {
        auto sub = GetIf<Operation>();
        if (!sub)
            return nullptr;
        return sub->template GetIf<T>();
    }

    template<is_alternative<Operation> T>
    T* GetIf() 
    {
        auto sub = GetIf<Operation>();
        if (!sub)
            return nullptr;
        return sub->template GetIf<T>();
    }

    template<is_alternative<Ordering> T>
    const T* GetIf() const 
    {
        auto sub = GetIf<Ordering>();
        if (!sub)
            return nullptr;
        return sub->template GetIf<T>();
    }

    template<is_alternative<Ordering> T>
    T* GetIf() 
    {
        auto sub = GetIf<Ordering>();
        if (!sub)
            return nullptr;
        return sub->template GetIf<T>();
    }

    template<is_alternative<Number> T>
    const T& Get() const 
    {
        return Get<Number>().template Get<T>();
    }

    template<is_alternative<Container> T>
    const T& Get() const 
    {
        return Get<Container>().template Get<T>();
    }

    template<is_alternative<Operation> T>
    const T& Get() const 
    {
        return Get<Operation>().template Get<T>();
    }

    template<is_alternative<Ordering> T>
    const T& Get() const 
    {
        return Get<Ordering>().template Get<T>();
    }

    template<is_alternative<Container> CT>
    bool operator==(const CT& rhs) const
    {
        return std::visit(overloaded_visit{
            [&rhs](const Container& sub)
            {
                return sub == rhs;
            }, 
            [](const auto&) { return false; }   
        }, *this);
    }    

    template<typename T>
    const std::optional<T> TryCast() const
    {
        return std::visit(overloaded_visit{
            [](const Container& container)
            {
                return container.TryCast<T>();
            },
            [](const Number& number)
            {
                return number.TryCast<T>();
            },
            [this](const auto&)
            {
                return ExpressionVariant::TryCast<T>();
            }
        }, *this);
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
    const std::type_info& AlternativeTypeId() const;
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
#include "Logic/Internal/FlatTupleImpl.h"
#include "Logic/Internal/OperationImpl.h"
#include "Logic/Internal/ComparisonImpl.h"
#include "Logic/Internal/UnaryOperate.h"
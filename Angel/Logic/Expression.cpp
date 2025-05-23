#include "Logic/Expression.h"
#include "Logic/Boolean.h"
#include "Logic/Integer.h"
#include "Logic/Id.h"
#include "Logic/Expression.h"
#include "Logic/Knowledge.h"
#include <sstream>
#include <cassert>

namespace Angel::Logic
{

Expression::Expression(const Expression& e) :
    ExpressionVariant(e)
{
}

template <typename... Ts>
ExpressionVariant make_unary_operation(const Operator ope, Expression&& operand) 
{
    if (!ope)
        return std::move(operand);
        
    Logic::Expression result = Logic::Boolean(false);
    bool found = false;
    
    ((!found && (found = (Ts::ope == ope)) 
        ? (result.emplace<Ts>(std::move(operand)), true) 
        : false), ...);
    
    if (!found) throw std::invalid_argument(std::format("Invalid unary operator {}", std::string(ope)));
    return result;
}

template <typename... Ts>
ExpressionVariant make_binary_operation(const Operator ope, Collection&& operands) 
{
    Logic::Expression result = Logic::Boolean(false);
    bool found = false;
    
    ((!found && (found = (Ts::ope == ope)) 
        ? (result.emplace<Ts>(std::move(operands)), true) 
        : false), ...);
    
    if (!found) throw std::invalid_argument(std::format("Invalid binary operator {}", std::string(ope)));
    return result;
}

ExpressionVariant make_operation(const Operator ope, Collection&& operands)
{
    if (operands.empty())
        throw std::invalid_argument(std::format("Not enough operands for operator {}", std::string(ope)));
    if (operands.size()==1) 
        return make_unary_operation<Negative>(ope, std::move(operands[0]));
    else 
        return make_binary_operation<Summation, Disjunction, Conjunction, Equation>(ope, std::move(operands));
}


Expression::Expression(const Operator ope, Collection&& operands) : 
    ExpressionVariant(make_operation(ope, std::move(operands)))
{
}

template <std::size_t idx>
constexpr Expression CastIterate(const Expression&e, const std::type_info& rtt) 
{
    if constexpr (idx < std::variant_size_v<ElementVariant>)
    {
        if (typeid(std::variant_alternative_t<idx, ElementVariant>) == rtt)
        {
            return e.Cast<std::variant_alternative_t<idx, ElementVariant>>();
        }
        else 
        {
            return CastIterate<idx+1>(e, rtt);
        }
    }
    return Expression();
}

Expression Expression::TryCast(const std::type_info& rtt) const
{
    return CastIterate<0>(*this, rtt);
}

Expression Expression::Cast(const std::type_info& rtt) const
{
    Expression result = CastIterate<0>(*this, rtt);
    if (result) 
        return result;
    else 
        throw CastException(AlternativeTypeId(), rtt); 
}

Expression Expression::Simplify() const
{
    return std::visit(overloaded_visit{
        [](std::monostate)  
        {
            return Expression();
        },
        [this]<IsElement T>(const T& element)   -> Expression 
        {
            return *this;
        },
        [this](const Function& f) -> Expression 
        {
            return *this;
        },
        [this](const auto& obj)
        {
            return obj.Simplify();
        }},   *this);      
}

Expression Expression::Substitute(const Variables& substitutions) const
{
    return std::visit(overloaded_visit{
        [](std::monostate)  
        {
            return Expression();
        },
        [this]<IsElement T>(const T& element)   -> Expression 
        {
            return *this;
        },
        [this](const Function& f) -> Expression 
        {
            return *this;
        },
        [&substitutions](const auto& obj) -> Expression
        {
            return obj.Substitute(substitutions);
        }},   *this);      
}


Match Expression::Matches(const Expression& e, const Variables& subs) const 
{
    return std::visit(overloaded_visit{
        [](std::monostate) -> Match 
        {
            assert(false);  // should never be matching against null-expression
            return Boolean(false);
        },
        [&e, &subs, this]<IsElement T>(const T& element) -> Match 
        {
            const auto* var = e.GetIf<Variable>();
            if (var)
                return var->Matches(*this, subs, true);
            if (e==element)
                return Boolean(true);            
            return Boolean(false); 
        },
        [&e, &subs](const auto& obj) -> Match 
        {
            return obj.Matches(e, subs);
        }},   *this);    
}

Expression Expression::Infer(const class Knowledge& knowledge, const Variables& vars) const
{
    return std::visit(overloaded_visit{
        [this](std::monostate)
        {
            return *this;
        },
        []<IsElement T>(const T& element) -> Expression {
            return element; 
        },
        [&knowledge, &vars](const auto& obj) -> Expression {
            return obj.Infer(knowledge, vars);
        }
    }, *this);
}

Expression::operator bool() const
{
    static_assert(std::is_same_v<std::variant_alternative_t<0, ExpressionVariant>, std::monostate>);
    return index() != 0;
}

bool Expression::operator==(const Expression& rhs) const
{
    return std::visit(overloaded_visit{
        [this](std::monostate)
        {
            return !bool(*this);
        },
        [this](const auto& rho)
        {
            return operator==(rho);
        }
        }, rhs);
}

bool Expression::operator<(const Expression& e) const
{
    std::hash<Expression> hasher; 
    return hasher(*this) < hasher(e);
}

const std::type_info& Expression::AlternativeTypeId() const 
{
    return *std::visit([](const auto& obj)
    {
        return &typeid(decltype(obj)); 
    }, *this);        
}


std::ostream& operator<<(std::ostream& s, const Expression& e)
{
    std::visit(overloaded_visit{
        [&s](std::monostate)
        {
            s << "null";
        },   
        [&s](const auto& obj)
        {
            s << obj;
        } 
    }, e);
    return s;
}

std::string to_string(const Expression& e)
{
    std::stringstream ss;
    ss << e;
    return ss.str();
}

}

namespace std
{

size_t hash<Angel::Logic::Expression>::operator()(const Angel::Logic::Expression& e) const
{
    return std::visit(Angel::Logic::overloaded_visit{
        [](std::monostate mono) -> size_t 
        { 
            std::hash<std::monostate> hasher;
            return hasher(mono);
        }, 
        [](const auto& obj) { return obj.Hash(); } 
    },  e);
}


}

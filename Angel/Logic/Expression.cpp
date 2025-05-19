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
        return make_binary_operation<Summation, Disjunction, Conjunction>(ope, std::move(operands));
}


Expression::Expression(const Operator ope, Collection&& operands) : 
    ExpressionVariant(make_operation(ope, std::move(operands)))
{
}

Match Expression::Matches(const Expression& e, const Variables& subs) const 
{
    return std::visit(overloaded_visit{
        [&e, &subs, this]<IsElement T>(const T& element) -> Match {
            const auto* var = std::get_if<Variable>(&e);
            // TODO this could be Variable::Matches()
            if (var)
                return var->Matches(*this, subs);
            if (e==element)
                return IsMatch;            
            return NoMatch; 
        },
        [&e, &subs](const auto& obj) -> Match 
        {
            return obj.Matches(e, subs);
        }},   *this);    
}

Expression Expression::Infer(const class Knowledge& knowledge, const Variables& vars) const
{
    return std::visit(overloaded_visit{
        []<IsElement T>(const T& element) -> Expression {
            return element; 
        },
        [&knowledge, &vars](const auto& obj) -> Expression {
            return obj.Infer(knowledge, vars);
        }
    }, *this);
}

bool Expression::operator<(const Expression& e) const
{
    std::hash<Expression> hasher; 
    return hasher(*this) < hasher(e);
}

std::ostream& operator<<(std::ostream& s, const Expression& e)
{
    std::visit([&s](const auto& obj)
    {
        s << obj;
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
    return std::visit([](const auto& obj) { return obj.Hash(); }, e);
}


}

#include "Logic/Expression.h"
#include "Logic/Boolean.h"

namespace Angel::Logic
{

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
        return make_binary_operation<Summation, Subtraction, Multiplication, Division, Disjunction, Conjunction, 
            Equal, Unequal, Lesser, LesserEqual,Greater, GreaterEqual>(ope, std::move(operands));
}


Expression::Expression(const Operator ope, Collection&& operands) : 
    ExpressionVariant(make_operation(ope, std::move(operands)))
{
}


}

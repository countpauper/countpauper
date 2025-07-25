#include "Logic/Expression.h"
#include "Logic/Boolean.h"

namespace Angel::Logic
{

template <typename... Ts>
ExpressionVariant make_unary_operation(const Operator ope, Expression&& operand) 
{
    Logic::Expression result = Logic::False;
    bool found = false;
    
    ((!found && (found = (Ts::ope == ope)) 
        ? (result.emplace<typename Ts::Pariant>(Ts(std::move(operand))), true) 
        : false), ...);
    
    if (!found) throw std::invalid_argument(std::format("Invalid unary operator {}", std::string(ope)));
    return result;
}

template <typename... Ts>
ExpressionVariant make_binary_operation(const Operator ope, Tuple&& operands) 
{
    if (operands.size()!=2)
        throw std::invalid_argument(std::format("Binary operation {} needs two operands, not {}", std::string(ope), operands.size()));

    Logic::Expression result = Logic::False;
    bool found = false;

    ((!found && (found = (Ts::ope == ope)) 
        ? result.emplace<typename Ts::Pariant>(Ts(std::move(operands[0]), std::move(operands[1]))), true 
        : false), ...);
    
    if (!found) throw std::invalid_argument(std::format("Invalid multiary operator {}", std::string(ope)));
    return result;
}

template <typename... Ts>
ExpressionVariant make_multiary_operation(const Operator ope, Tuple&& operands) 
{
    Logic::Expression result = Logic::False;
    bool found = false;

    ((!found && (found = (Ts::ope == ope)) 
        ? result.emplace<Operation>(Ts(std::move(operands))), true 
        : false), ...);
    
    if (!found) throw std::invalid_argument(std::format("Invalid multiary operator {}", std::string(ope)));
    return result;
}

template <typename... Ts>
ExpressionVariant make_ordering(Operator ope, Tuple&& operands) 
{
    if (ope.Operands()==1)
        ope=Comparator(ope.op.sw.unicode);  // Find operator defined with comparators, also for filters
    Logic::Expression result = Logic::False;
    bool found = false;
    ((!found && (found = (Ts::ope == ope)) 
        ? result.emplace<Ordering>(Ts(std::move(operands))), true 
        : false), ...);    
    if (!found) throw std::invalid_argument(std::format("Invalid comparator {}", std::string(ope)));
    return result;
}

ExpressionVariant make_operation(const Operator ope, Tuple&& operands)
{
    if (operands.empty())
        throw std::invalid_argument(std::format("Not enough operands for operator {}", std::string(ope)));
    if (!ope)
    {
        assert(operands.size() == 1);
        return std::move(operands[0]);
    }
    if (ope.IsComparator())
        return make_ordering<Equal, Unequal, Lesser, LesserEqual,Greater, GreaterEqual>(ope, std::move(operands));
    else if (ope.Operands()==1)
            return make_unary_operation<All, Negative, Negation, Variable>(ope, std::move(operands[0]));
    else if (ope.Operands()==2)
        return make_binary_operation<Item, Association>(ope, std::move(operands));
    else    
        return make_multiary_operation<Summation, Subtraction, Multiplication, Division, Disjunction, Conjunction, Exclusion>(ope, std::move(operands));
}


Expression::Expression(const Operator ope, Tuple&& operands) : 
    ExpressionVariant(make_operation(ope, std::move(operands)))
{
}


}

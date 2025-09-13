#include "Logic/NewUnaryOperator.h"
#include "Logic/Expression.h"

namespace Angel::Logic
{

std::map<NewOperator::Code, NewOperator*> NewOperator::all;

NewUnaryOperator::NewUnaryOperator(wchar_t c) 
    : NewOperator(c, 1) 
{
}

Tuple NewUnaryOperator::operator()(const Tuple& operands) const
{
    if (operands.empty())
        throw std::invalid_argument(std::format("Missing operand for unary operator {}", std::string(*this)));
    if (operands.size()>1)
        throw std::invalid_argument(std::format("Too many operands ({}) for unary operator {}", operands.size(), std::string(*this)));
    return Tuple{(*this)(operands.front())};
}

unsigned NewUnaryOperator::MinimumOperands() const 
{ 
    return 1; 
}


void NewUnaryOperator::AddOperand(std::string& str, const Expression& operand) const
{
    std::string result;
    if (NeedsBracesAround(operand, op.sw.postfix))
        result = std::format("({})", to_string(operand));
    else 
        result = to_string(operand);
    if (op.sw.postfix)
    {
        str += result+ UnicodeToUtf8(op.sw.unicode);
    }
    else 
    {
        str += UnicodeToUtf8(op.sw.unicode) + result;
    }    
}

const NewUnaryOperator* NewUnaryOperator::Unary() const
{
    return nullptr;
}


void NewUnaryOperator::SelfInvertible()
{
    inverse = this;
}

NewUnaryOperator* NewUnaryOperator::FindInverse(wchar_t tag) const
{
    auto inv_op=all.find(Code{.sw{tag, 1, IsPostfix(), IsComparator(), 0, 0}});    
    if (inv_op==all.end())
        inv_op=all.find(Code{.sw{tag, 1, !IsPostfix(), IsComparator(), 0, 0}});    
    if (inv_op==all.end())
        return nullptr;
    return static_cast<NewUnaryOperator*>(inv_op->second);
}

bool NewUnaryOperator::SetInvertible(wchar_t inversion)
{
    auto foundInverse = FindInverse(inversion);
    if (!foundInverse) 
        return false;
    inverse = foundInverse;
    foundInverse->inverse = this;
    return true;
}

GenericOperation NewUnaryOperator::InvertLeft(Expression&& lhs, Expression&& rhs) const
{
    if (!inverse)
        return GenericOperation();  // not invertible 

    assert(!rhs);   // Should only provide one
    assert(inverse->IsUnary());
    return GenericOperation(*inverse, std::move(lhs));
} 

GenericOperation NewUnaryOperator::InvertRight(Expression&& lhs, Expression&& rhs) const
{
    return InvertLeft(std::move(rhs), std::move(lhs));
}


}
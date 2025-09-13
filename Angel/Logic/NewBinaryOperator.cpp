#include "Logic/NewBinaryOperator.h"
#include "Logic/Expression.h"

namespace Angel::Logic
{


NewBinaryOperator::NewBinaryOperator(wchar_t code, bool multiary) 
    : NewOperator(code, multiary?3:2) 
{
}

NewBinaryOperator::~NewBinaryOperator() = default;


Tuple NewBinaryOperator::operator()(const Tuple& operands) const
{
    if ((operands.size()==1) && (unary))
    {
        return (*unary)(operands);
    }
    Expression constant;
    Tuple result;
    if (right_associative) 
        result = Tuple(std::ranges::reverse_view(operands));
    else
        result = operands;

    auto it = result.begin();
    while(it!=result.end())
    {
        if (it->IsConstant())
        {
            if (absorb)
            {
                if (*it == *absorb)
                    return Tuple{*absorb};
            }
            if (identity) 
            {
                if (*it == *identity)
                {
                    it = result.erase(it);
                    continue;
                }
            }
            if (constant)
                if (right_associative)
                    constant = (*this)(*it, constant);
                else 
                    constant = (*this)(constant, *it);
            else
                constant = *it;
            it = result.erase(it);
        }
        else 
            ++it;
    }
    if (constant) 
    {
        result.AddAt(result.cbegin(), std::move(constant));
    }
    if (identity && result.empty())
    {
        return Tuple{*identity};
    }
    return result;
}

unsigned NewBinaryOperator::MinimumOperands() const 
{ 
    if (identity)
        return 0;
    else if (unary)
        return 1;
    else 
        return 2;
}

void NewBinaryOperator::AddOperand(std::string& str, const Expression& operand) const
{
    bool first = str.empty();
    if (!first)
        str += UnicodeToUtf8(op.sw.unicode);
    
    if (NeedsBracesAround(operand, first))
        str += std::format("({})", to_string(operand));
    else 
        str += to_string(operand);
}

const NewUnaryOperator* NewBinaryOperator::Unary() const
{
    return unary;
}

NewBinaryOperator* NewBinaryOperator::FindInverse(wchar_t tag) const
{
    auto inv_op=all.find(Code{.sw{tag, op.sw.operands, false, IsComparator(), 0, 0}});    
    if (inv_op==all.end())
        return nullptr;
    return static_cast<NewBinaryOperator*>(inv_op->second);
}

bool NewBinaryOperator::SetCommutativeInvertible(wchar_t inversion)
{
    auto foundInverse= FindInverse(inversion);

    // it's not an error if not found. One of the inversions is constructed first. 
    // if not found postpone and the inversion will make the circular reference
    if (!foundInverse)
        return false;
    inverseLeft = foundInverse;
    inverseRight = foundInverse;
    if (foundInverse->inverseLeft == nullptr)
        foundInverse->inverseLeft = this;
    if (foundInverse->inverseRight == nullptr)
        foundInverse->inverseRight = this;
    return true;    
}

bool NewBinaryOperator::SetInvertible(wchar_t leftInversion, wchar_t rightInversion)
{
    auto foundLeft = FindInverse(leftInversion); 
    auto foundRight = FindInverse(rightInversion);
    inverseLeft = foundLeft;
    inverseRight = foundRight;
    if (foundLeft && inverseLeft->inverseLeft == nullptr)
    {
        foundLeft->inverseLeft = this;
    }
    if (foundRight && inverseRight->inverseRight == nullptr)
    {
        foundRight->inverseRight = this;
    }
    return inverseLeft && inverseRight;
}

GenericOperation NewBinaryOperator::InvertLeft(Expression&& lhs, Expression&& rhs) const
{
    if (!rhs && unary)
        return unary->InvertLeft(std::move(lhs), std::move(rhs));
    if (!inverseLeft)
        return GenericOperation();
    return GenericOperation(*inverseLeft, Tuple{ std::move(lhs), std::move(rhs)});    // TODO if rhs is a generic operation of the same type, then add all the elements
} 

GenericOperation NewBinaryOperator::InvertRight(Expression&& lhs, Expression&& rhs) const
{
    if (!lhs && unary)
        return unary->InvertRight(std::move(lhs), std::move(rhs));
    if (!inverseRight)
        return GenericOperation();
    return GenericOperation(*inverseRight, Tuple{ std::move(lhs), std::move(rhs)});    // TODO if lhs is a generic operation of the same type, then add all the elements
}

}
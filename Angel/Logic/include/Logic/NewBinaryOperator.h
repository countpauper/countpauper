#pragma once

#include "Logic/NewOperator.h"

namespace Angel::Logic
{

class NewBinaryOperator : public NewOperator 
{
public:
    NewBinaryOperator(wchar_t code, bool multiary=false);
    ~NewBinaryOperator();
    virtual Expression operator()(const Expression& lhs, const Expression& rhs) const = 0;
    Tuple operator()(const Tuple& operands) const override;
    unsigned MinimumOperands() const override;
    void AddOperand(std::string& str, const Expression& operand) const override;
    static inline const NewBinaryOperator& Find(wchar_t tag) { return static_cast<const NewBinaryOperator&>(NewOperator::Find(tag, false)); }
    const NewUnaryOperator* Unary() const override;
    bool SetCommutativeInvertible(const wchar_t inversion);
    bool SetInvertible(const wchar_t leftInversion, const wchar_t rightInversion);
    GenericOperation InvertLeft(Expression&& lhs, Expression&& rhs) const override; 
    GenericOperation InvertRight(Expression&& lhs, Expression&& rhs) const override; 
protected:
    NewBinaryOperator* FindInverse(wchar_t tag) const;
    
    const NewUnaryOperator* unary;
    const NewBinaryOperator* inverseLeft=nullptr;
    const NewBinaryOperator* inverseRight=nullptr;
    bool right_associative = false;
    const Expression* identity;
    const Expression* absorb;
};

std::string UnicodeToUtf8(wchar_t tag);

}


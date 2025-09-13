#pragma once

#include "Logic/NewOperator.h"

namespace Angel::Logic
{
class Expression;
class GenericOperation;

class NewUnaryOperator : public NewOperator
{
public:
    explicit NewUnaryOperator(wchar_t c);
    Tuple operator()(const Tuple& operands) const override;
    unsigned MinimumOperands() const override;
    virtual Expression operator()(const Expression& operand) const = 0;
    void AddOperand(std::string& str, const Expression& operand) const override;
    const NewUnaryOperator* Unary() const override;
    void SelfInvertible();
    bool SetInvertible(const wchar_t leftInversion);
    GenericOperation InvertLeft(Expression&& lhs, Expression&& rhs) const override; 
    GenericOperation InvertRight(Expression&& lhs, Expression&& rhs) const override; 
    static inline const NewUnaryOperator& Find(wchar_t tag) { return static_cast<const NewUnaryOperator&>(NewOperator::Find(tag, true)); }
protected:
    NewUnaryOperator* FindInverse(wchar_t tag) const;
    const NewUnaryOperator* inverse=nullptr;

};

}


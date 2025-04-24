#pragma once
#include "Object.h"
#include "Expression.h"
#include <vector>

namespace Angel
{
namespace Logic
{

class Knowledge;

// An Operator is the basis of all functions
// the Operators construct a hierachy of Objects by combining one or more of them with an operation

class Operator : public Expression // TODO: objects also expressions that just return themselves when infered? 
{
public:
    Operator() = default;
    virtual ~Operator() = default;
    Match Matching(const Expression& other, const Variables& substitutions) const;
    const Object* Condition() const override;
};

// The Unary Operator performs a function on a single Object
class Unary : public Operator
{
public:
    Unary() = default;
    Unary(Object&& operand) : operand(std::move(operand)) {}
    std::size_t Hash() const override;
protected:
    Object operand;
};

// The Binary Operator performs a function on exactle two Objects, which typically have an asymetrical relation 
class Binary : public Operator
{
public:
    Binary() = default;
    Binary(Object&& left, Object&& right) : left(std::move(left)), right(std::move(right)) {}
    std::size_t Hash() const override;
protected:
    Object left;
    Object right;
};

// TODO: Narys are colllections, not owners of a set, sequences are narys with a collection operator, 
// The N-ary Operator performs a function on a sequence of zero or more Objects, which typically have a symetric relation 
class Nary : public Operator
{
public:
    Nary() = default;
    void Add(Object&& value);
    operator bool() const override;
    std::size_t Hash() const override;
protected:
    using Operands = std::vector<Object>;
    Nary(const Operands& other);
    Operands operands;
};



}
}
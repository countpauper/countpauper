#pragma once
#include "Object.h"
#include "Array.h"

namespace Angel
{
namespace Logic
{

class Knowledge;

// An Operator is the basis of all functions
// the Operators construct a hierachy of Objects by combining one or more of them with an operation

class Operator : public Expression // TODO: objects also expressions that just return themselves when computed? 
{
public:
    Operator() = default;
    virtual ~Operator() = default;
};

// The Unary Operator performs a function on a single Object
class Unary : public Operator
{
public:
    Unary() = default;
    Unary(Object&& operand) : operand(std::move(operand)) {}
protected:
    Object operand;
};

// The Binary Operator performs a function on exactle two Objects, which typically have an asymetrical relation 
class Binary : public Operator
{
public:
    Binary() = default;
    Binary(Object&& left, Object&& right) : left(std::move(left)), right(std::move(right)) {}
protected:
    Object left;
    Object right;
};

// The N-ary Operator performs a function on a set of zero or more Objects, which typically have a symetric relation 
class Nary : public Operator
{
public:
    Nary() = default;
    Nary(Array&& operands) : operands(std::move(operands)) {} 
protected:
    Array operands;
};



}
}
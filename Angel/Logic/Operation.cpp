#include "Logic/Internal/Operation.h"
#include "Logic/Expression.h"
#include <sstream>
#include <string>

namespace Angel::Logic
{

Operation& Operation::operator=(const Operation& e)
{
    OperationVariant::operator=(e);
    return *this;
}

std::size_t Operation::size() const
{
    return std::visit(
        [this](const auto& obj)
        {
            return obj.size();
        }, *this);   
}

Expression Operation::Simplify() const
{
    return std::visit(
        [this](const auto& obj) -> Expression
        {
            return obj.Simplify();
        }, *this);      
}

Set Operation::Assumptions() const
{
    return std::visit(
        [this](const auto& obj)
        {
            return obj.Assumptions();
        }, *this);

}

Expression Operation::Substitute(const Hypothesis& hypothesis) const
{
    return std::visit(
        [&hypothesis](const auto& obj) -> Expression
        {
            return obj.Substitute(hypothesis);
        }, *this);      
}


Expression Operation::Matches(const Expression& e, const Hypothesis& hypothesis) const 
{
    return std::visit(
        [&e, &hypothesis](const auto& obj) 
        {
            return obj.Matches(e, hypothesis);
        },   *this);    
}

Expression Operation::Infer(const class Knowledge& knowledge, const Hypothesis& hypothesis, Trace& trace) const
{
    return std::visit(
        [&knowledge, &hypothesis, &trace](const auto& obj) -> Expression {
            return obj.Infer(knowledge, hypothesis, trace);
        }, *this);
}

Operator Operation::GetOperator() const
{
    return std::visit(
        [](const auto& obj) -> Operator
        {
            return obj.ope;
        }, *this);    
}


Expression Operation::RemoveLeft()
{
    return std::visit(
        [](auto& obj) -> Expression
        {
            return obj.RemoveLeft();
        }, *this);
}

void Operation::AddLeft(Expression&& operand)
{
    std::visit(
        [&operand](auto& obj)
        {
            obj.AddLeft(std::move(operand));
        }, *this);
}

Expression Operation::RemoveRight()
{
    return std::visit(
        [](auto& obj) -> Expression
        {
            return obj.RemoveRight();
        }, *this);
}

void Operation::AddRight(Expression&& operand)
{
    std::visit(
        [&operand](auto& obj)
        {
            obj.AddRight(std::move(operand));
        }, *this);
}

std::string Operation::Summary() const
{
    auto terms = size();
    if (terms==1)
        return std::format("{} with {} term", GetOperator().Description(), terms);
    else 
        return std::format("{} with {} terms", GetOperator().Description(), terms);
}    

bool Operation::operator==(const Operation& rhs) const
{
    return std::visit(
        [this](const auto& rho)
        {
            return operator==(rho);
        }, rhs);
}


bool Operation::operator<(const Operation& o) const
{
    return Hash() < o.Hash();
}

std::string to_string(const Operation& c)
{
    std::stringstream ss;
    ss << c;
    return ss.str();
}

std::ostream& operator<<(std::ostream& s, const Operation& c)
{
    std::visit(
        [&s](const auto& obj)
        {
            s << obj;
        }, c);
    return s;
}


}
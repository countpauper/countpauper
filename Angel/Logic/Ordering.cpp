#include "Logic/Internal/Ordering.h"
#include "Logic/Expression.h"
#include <sstream>
#include <string>

namespace Angel::Logic
{

Ordering& Ordering::operator=(const Ordering& e)
{
    OrderingVariant::operator=(e);
    return *this;
}

std::size_t Ordering::size() const
{
    return std::visit(
        [this](const auto& obj)
        {
            return obj.size();
        }, *this);   
}

Expression Ordering::Simplify() const
{
    return std::visit(
        [this](const auto& obj) -> Expression
        {
            return obj.Simplify();
        }, *this);      
}

Set Ordering::Assumptions() const
{
    return std::visit(
        [this](const auto& obj)
        {
            return obj.Assumptions();
        }, *this);
}

bool Ordering::HasLeftAssumption() const
{
    return std::visit(    
        [this](const auto& obj)
        {
            return obj.HasLeftAssumption();
        }, *this);   
}


Ordering Ordering::Substitute(const Hypothesis& hypothesis) const
{
    return std::visit(
        [&hypothesis](const auto& obj) -> Ordering
        {
            return Ordering(obj.Substitute(hypothesis));
        },   *this);      
}


Expression Ordering::Matches(const Expression& e, const Hypothesis& hypothesis) const 
{
    return std::visit(
        [&e, &hypothesis](const auto& obj) 
        {
            return obj.Matches(e, hypothesis);
        },   *this);    
}

Expression Ordering::Infer(const class Knowledge& knowledge, const Hypothesis& hypothesis, Trace& trace) const
{
    return std::visit(
        [&knowledge, &hypothesis, &trace](const auto& obj) -> Expression {
            return obj.Infer(knowledge, hypothesis, trace);
        }, *this);
}

Ordering::operator bool() const
{
    return size()!=0;
}

Comparator Ordering::GetComparator() const
{
    return std::visit(
        [](const auto& obj) 
        {
            return obj.ope;
        },   *this);    
}

std::string Ordering::Summary() const
{
    return std::format("{} with {} operands", GetComparator().Description(), size());
}    

bool Ordering::operator==(const Ordering& rhs) const
{
    return std::visit(
        [this](const auto& rho)
        {
            return operator==(rho);
        }, rhs);
}


bool Ordering::operator<(const Ordering& o) const
{
    return Hash() < o.Hash();
}

std::string to_string(const Ordering& c)
{
    std::stringstream ss;
    ss << c;
    return ss.str();
}

std::ostream& operator<<(std::ostream& s, const Ordering& c)
{

    std::visit(
        [&s](const auto& obj)
        {
            // TODO: operand to string with GetOperator()
            s << obj;
        }, c);
    return s;
}

}
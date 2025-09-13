#pragma once

#include "Logic/Internal/Tuple.h"
#include "NewOperator.h"
#include "NewUnaryOperator.h"
#include "NewBinaryOperator.h"
#include <initializer_list>
#include <ranges>
#include <map>
#include <format>

namespace Angel::Logic
{
class Expression;

class GenericOperation : public Tuple
{
public:
    GenericOperation();
    GenericOperation(const GenericOperation&) = default;
    GenericOperation(const NewUnaryOperator& ope, const Expression& operand);
    GenericOperation(const NewUnaryOperator& ope, Expression&& operand);
    GenericOperation(wchar_t tag, Expression&& operand);

    template <std::ranges::input_range R>
	GenericOperation(const NewOperator& ope, R operands) 
        : ope(ope)
    {

        if (operands.size() < ope.MinimumOperands())
            throw std::invalid_argument(std::format("{} needs at least {} operands, not {}", ope.Description(), ope.MinimumOperands(), operands.size()));
 
        for(auto& operand: operands) 
        {
            Add(std::move(operand));
        }        
    }

    template <std::ranges::input_range R>
	GenericOperation(wchar_t tag, R operands) 
        : GenericOperation(NewBinaryOperator::Find(tag), std::move(operands))
    {
    }

    unsigned Add(Expression&& operand);
    unsigned Add(const Expression& operand);

    GenericOperation& operator=(const GenericOperation& e);

    Expression Simplify() const;
    Expression Substitute(const Hypothesis& hypothesis) const;
    Expression Matches(const Expression& e, const Hypothesis& hypothesis) const;
    Expression Infer(const class Knowledge& knowledge, const Hypothesis& hypothesis, Trace& trace) const;
    const NewOperator& GetOperator() const;
    GenericOperation Solve(const Expression& target, Expression&& substitute) const;
    std::string Summary() const;

    bool operator==(const GenericOperation& rhs) const;
    explicit operator bool() const; // not none operator

    template<typename T> 
    bool operator<(const T& rhs) const
    {
        std::hash<Expression> hasher;
        return Hash() < hasher(rhs);
    }
    std::size_t Hash() const;
    operator std::string() const;
protected:
    using Base=Tuple;
    void AddOperand(std::string& str, const Expression& e) const;
private:

    const NewOperator& ope; 
};

std::ostream& operator<<(std::ostream& s, const GenericOperation& e);
// This helper is needed because google test printer prioritizes begin() end() 
void PrintTo(const GenericOperation&, ::std::ostream* os);

}

#include "Logic/Summation.h"
#include "Logic/Negation.h"
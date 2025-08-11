#pragma once

#include "Logic/Internal/Tuple.h"
#include "Logic/NewOperator.h"
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
    GenericOperation(const NewOperator& ope, std::initializer_list<Expression> operands);
    GenericOperation(const NewOperator& ope, Tuple&& operands);
    GenericOperation(const NewUnaryOperator& ope, Expression&& operand);
    GenericOperation(wchar_t tag, Tuple&& operands);
    GenericOperation(wchar_t tag, Expression&& operand);

    template <std::ranges::input_range R>
	explicit GenericOperation(Operator ope, R operands) 
        : ope(ope)
    {

        if (ope.Operands() < 3) 
        {
            if (operands.size() != ope.Operands())
                throw std::invalid_argument(std::format("Operation {} needs {} operands, not {}", ope, ope.Operands(), operands.size()));
        }
        else 
        {
            if (!bool(ope.Identity()) && operands.empty())
                throw std::invalid_argument(std::format("Operands list of {} can not be empty", ope));
        } 
        for(auto& operand: operands) 
        {
            Add(std::move(operand));
        }        
    }

    unsigned Add(Expression&& operand);
    unsigned Add(const Expression& operand);

    GenericOperation& operator=(const GenericOperation& e);

    Expression Simplify() const;
    Expression Substitute(const Hypothesis& hypothesis) const;
    Expression Matches(const Expression& e, const Hypothesis& hypothesis) const;
    Expression Infer(const class Knowledge& knowledge, const Hypothesis& hypothesis, Trace& trace) const;
    const NewOperator& GetOperator() const;
    GenericOperation Solve(const Expression& target, const Expression& substitute) const;
    std::string OperandToString(const Expression& e, bool first) const;
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
protected:
    using Base=Tuple;
private:

    const NewOperator& ope; 
};

std::ostream& operator<<(std::ostream& s, const GenericOperation& e);


}

#include "Logic/Summation.h"
#include "Logic/Negation.h"
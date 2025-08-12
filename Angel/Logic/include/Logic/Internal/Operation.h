#pragma once

#include "Logic/Negative.h"
#include "Logic/Subtraction.h"
#include "Logic/Multiplication.h"
#include "Logic/Division.h"
#include "Logic/Exponentiation.h"
#include "Logic/Logarithm.h"

#include "Logic/Negation.h"
#include "Logic/Conjunction.h"
#include "Logic/Disjunction.h"
#include "Logic/Exclusion.h"
#include "Logic/Variable.h"
#include "Logic/Item.h"
#include "Logic/Internal/Variant.h"
#include <variant>


namespace Angel::Logic
{

using OperationVariant = Variant<
    Variable, Item,
    Multiplication, Division, Exponentiation, Logarithm,
    Conjunction, Disjunction, Exclusion>;  
class const_container_iterator;

class Operation : public OperationVariant
{
public:
    Operation(const Operation&) = default;
    using OperationVariant::OperationVariant;
    
    Operation& operator=(const Operation& e);

    std::size_t size() const;
    Expression Simplify() const;
    Set Assumptions() const;
    Expression Substitute(const Hypothesis& hypothesis) const;
    Expression Matches(const Expression& e, const Hypothesis& hypothesis) const;
    Expression Infer(const class Knowledge& knowledge, const Hypothesis& hypothesis, Trace& trace) const;
    Operator GetOperator() const;
    Expression RemoveLeft();
    void AddLeft(Expression&& operand);
    Expression RemoveRight();
    void AddRight(Expression&& operand);
    std::string Summary() const;

    template<typename T> 
    requires is_alternative<T, OperationVariant>
    bool operator==(const T& rhs) const
    {
        return std::visit(overloaded_visit{
            [&rhs](const T& lv)
            {
                return lv == rhs;
            }, 
            [](const auto&) { return false; }   
            },*this);
    }
    bool operator==(const Operation& rhs) const;
 
    template<typename T> 
    bool operator<(const T& rhs) const
    {  
        std::hash<Expression> hasher;
        return this->Hash() < hasher(rhs);
    }

    bool operator<(const Operation& o) const;
};

std::ostream& operator<<(std::ostream& s, const Operation& e);
}

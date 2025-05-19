#pragma once
#include "Logic/Match.h"
#include "Logic/Operator.h"
#include <memory>

namespace Angel::Logic
{

class Expression;

class Association
{
public:
    Association(const Association& o);
    Association(Association&& o); 
    Association(Expression&& lhs, Expression&& rhs);
    Association& operator=(const Association& o);
    Association& operator=(Association&& o);

    explicit operator bool() const;
    std::size_t size() const;
    const Expression& Left() const;
    const Expression& Right() const;
    Match Matches(const Expression& expression, const Variables& vars) const;
    Expression Infer(const class Knowledge& k, const Variables& substitutions) const;
    Expression Get(const Expression& key) const;
    bool operator==(const Association& other) const;
    std::size_t Hash() const;
    constexpr static BinaryOperator ope{L':'};
private:
    std::unique_ptr<Expression> lhs;
    std::unique_ptr<Expression> rhs;
};


std::ostream& operator<<(std::ostream& os, const Association& a);

}
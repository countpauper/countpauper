#pragma once
#include "Logic/Match.h"
#include "Logic/Operator.h"
#include <memory>

namespace Angel::Logic
{

class Expression;
class Object;

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
    Object Infer(const class Knowledge& k, const Variables& substitutions) const;
    Expression Get(const Expression& key) const;
    bool operator==(const Association& other) const;
    std::size_t Hash() const;
    constexpr static BinaryOperator ope{L':'};
private:
    std::unique_ptr<Expression> lhs;
    std::unique_ptr<Expression> rhs;
};


std::ostream& operator<<(std::ostream& os, const Association& a);
    /*
// A clause is an association (TODO: rename) expresion that specifies that a predicate is true if all of the array of its conditions is true
class Clause
{
public:
	explicit Clause(Predicate&& predicate, Expression&& condition = boolean(true));
    explicit operator bool() const override;
	bool operator==(const Clause& other) const;
    std::size_t Hash() const;
    Match Matching(const Expression& other, const Variables& substitutions) const;
    const Object* Condition() const;    // TODO right
    Object Copy() const;
    Object Infer(const Knowledge& known, const Variables& substitutions) const;
protected:
    Object Cast(const std::type_info& t, const Knowledge& k) const;
private:
    friend std::ostream& operator<<(std::ostream& os, const Clause& );
	Predicate predicate;
    Object condition;
};



Object clause(Predicate&& id, Object&& condition = boolean(true));

*/

}
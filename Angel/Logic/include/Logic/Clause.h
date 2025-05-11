#pragma once
#include "Object.h"
#include "Predicate.h"
#include "Boolean.h"
#include "Expression.h"

namespace Angel::Logic
{

    /*
// A clause is a pair (TODO: rename) expresion that specifies that a predicate is true if all of the array of its conditions is true
class Clause
{
public:
	explicit Clause(Predicate&& predicate, Expression&& condition = boolean(true));
    operator bool() const override;
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

std::ostream& operator<<(std::ostream& os, const Clause& );

Object clause(Predicate&& id, Object&& condition = boolean(true));

*/

}
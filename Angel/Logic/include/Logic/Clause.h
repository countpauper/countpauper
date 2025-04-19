#pragma once
#include "Object.h"
#include "Predicate.h"
#include "Boolean.h"
#include "Expression.h"

namespace Angel
{
namespace Logic
{

// A clause is an expresion that specifies that a predicate is true if all of the array of its conditions is true
class Clause : public Expression
{
public:
	explicit Clause(Predicate&& predicate, Object&& condition = boolean(true));
    operator bool() const override;
	bool operator==(const Expression& other) const override;
    std::string String() const override;
    Object Match(const Expression& other) const override;
    Object Copy() const override;
    Object Compute(const Knowledge& known) const override;
protected:
    Object Cast(const std::type_info& t, const Knowledge& k) const override;
private:
	Predicate predicate;
    Object condition;
};


Object clause(Predicate&& id, Object&& condition = boolean(true));

}
}
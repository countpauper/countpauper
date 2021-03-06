#pragma once
#include "Object.h"
#include "Predicate.h"
#include "Array.h"
#include "Expression.h"

namespace Angel
{
namespace Logic
{

class Clause : public Expression
{
public:
	explicit Clause(Predicate&& predicate, Array&& conditions = Array());
	bool operator==(const Item& other) const override;
	bool Match(const Item& other, const Knowledge& knowledge) const override;
private:
	Predicate predicate;
	Array conditions;
};

Object clause(Predicate&& id, Array&& conditions=Array());

}
}
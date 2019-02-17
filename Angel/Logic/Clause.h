#pragma once
#include "Element.h"
#include "Predicate.h"
#include "Array.h"

namespace Angel
{
namespace Logic
{

class Clause : public Value
{
public:
	explicit Clause(Predicate&& predicate, Array&& conditions = Array());
	bool operator==(const Value& other) const override;
	bool Match(const Value& other, const Knowledge& knowledge) const override;
private:
	Predicate predicate;
	Array conditions;
};

Element clause(Predicate&& id, Array&& conditions=Array());

}
}
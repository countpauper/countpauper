#pragma once
#include "Element.h"
#include "Predicate.h"
#include "Sequence.h"

namespace Angel
{
namespace Logic
{

class Clause : public Value
{
public:
	explicit Clause(Predicate&& predicate, Sequence&& conditions = Sequence());
	bool operator==(const Value& other) const override;
	bool Match(const Value& other, const Knowledge& knowledge) const override;
private:
	Predicate predicate;
	Sequence conditions;
};

Element clause(Predicate&& id, Sequence&& conditions=Sequence());

}
}
#pragma once
#include "Element.h"
#include "Id.h"
#include "Sequence.h"

namespace Angel
{
namespace Logic
{

class Predicate : public Value
{
public:
	explicit Predicate(const Id& id, Sequence&& parameters=Sequence());
	explicit Predicate(const std::wstring& name, Sequence&& parameters=Sequence());
	bool operator==(const Value& other) const override;
	bool Match(const Value& other, const Knowledge& knowledge) const override;
private:
    Id id;
	Sequence parameters;
};

Element predicate(const Id& id, Sequence&& sequence);
Element predicate(const std::wstring& name, Sequence&& sequence);
Element predicate(const std::wstring& name);

}
}
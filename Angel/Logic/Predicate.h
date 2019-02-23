#pragma once
#include "Object.h"
#include "Id.h"
#include "Sequence.h"

namespace Angel
{
namespace Logic
{

class Predicate : public Item
{
public:
	explicit Predicate(const Id& id, Sequence&& parameters=Sequence());
	explicit Predicate(const std::wstring& name, Sequence&& parameters=Sequence());
	bool operator==(const Item& other) const override;
	bool Match(const Item& other, const Knowledge& knowledge) const override;
	void Argue(Object&& value);
private:
    Id id;
	Sequence arguments;
};

Object predicate(const Id& id, Sequence&& arguments=Sequence());
Object predicate(const std::wstring& name, Sequence&& arguments=Sequence());

}
}
#pragma once
#include "Object.h"
#include "Id.h"
#include "Sequence.h"

namespace Angel
{
namespace Logic
{

class Predicate : public Expression
{
public:
	explicit Predicate(const Id& id, Sequence&& parameters=Sequence());
	explicit Predicate(const std::wstring& name, Sequence&& parameters=Sequence());
	bool operator==(const Expression& other) const override;
	bool Match(const Expression& other, const Knowledge& knowledge) const override;
    Object Compute(const Knowledge& known) const override;
    void Argue(Object&& value);
protected:
    Object Cast(const std::type_info& t, const Knowledge& k) const override;
private:
    Id id;
	Sequence arguments;
};

Object predicate(const Id& id, Sequence&& arguments=Sequence());
Object predicate(const std::wstring& name, Sequence&& arguments=Sequence());

}
}
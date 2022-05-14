#include "stdafx.h"
#include <algorithm>
#include "Predicate.h"
#include "Boolean.h"
#include "Knowledge.h"

namespace Angel
{
namespace Logic
{

Predicate::Predicate(const Id& id, Sequence&& arguments) :
	id(id),
	arguments(std::move(arguments))
{
}

Predicate::Predicate(const std::wstring& tag, Sequence&& arguments) :
	id(tag),
	arguments(std::move(arguments))
{
}
bool Predicate::operator==(const Expression& other) const
{
	if (auto predicate = dynamic_cast<const Predicate*>(&other))
	{
		return id == predicate->id && arguments == predicate->arguments;
	}
	return false;
}

bool Predicate::Match(const Expression& expr, const Knowledge& knowledge) const
{
	if (auto predicate = dynamic_cast<const Predicate*>(&expr))
	{
		if (id != predicate->id)
			return false;
		return arguments.Match(predicate->arguments, knowledge);
	}
	return false;
}

Object Predicate::Compute(const Knowledge& known) const
{
    return boolean(known.Query(*this));
}

Object Predicate::Cast(const std::type_info& t, const Knowledge& k) const
{
    if (typeid(t) == typeid(Boolean))
        return Compute(k);
    throw CastException<Predicate>(t);
}

void Predicate::Argue(Object&& value)
{
	arguments.Append(std::move(value));
}

Object predicate(const Id& id, Sequence&& arguments)
{
	return Create<Predicate>(id, std::move(arguments));
}

Object predicate(const std::wstring& name, Sequence&& arguments)
{
	return Create<Predicate>(Id(name), std::move(arguments));
}

}
}
#include "stdafx.h"
#include "Set.h"
#include <algorithm>
#include "Sequence.h"
#include "Boolean.h"

namespace Angel
{
namespace Logic
{

Set::Set() = default;

Set::Set(const Set& set)
{
    for (const auto& si : set)
        emplace(si->Copy());
}


Set::Set(Sequence&& array)
{
	for (auto& e : array)
	{
		Append(std::move(e));
	}
}

/*  // this is weird, either added as element or cast implicity?
Set::Set(Object&& value)
{
	if (auto array = value.As<Sequence>())
	{
		for (auto& e : *array)
		{
			emplace(std::move(e));
		}
	}
	else if (value)
	{
		emplace(std::move(value));
	}
}
*/

Set::Set(Set&& other) :
	std::unordered_set<Object>(std::move(other))
{
}


Object Set::Copy() const
{
    return Create<Set>(*this);
}


bool Set::operator==(const Expression& other) const
{
	if (auto set = dynamic_cast<const Set*>(&other))
	{
		return std::operator==(static_cast<const std::unordered_set<Object>&>(*this),
			static_cast<const std::unordered_set<Object>&>(*set));
	}
	return false;
}

bool Set::Match(const Expression& expr, const Knowledge& knowledge) const
{
	if (auto set = dynamic_cast<const Set*>(&expr))
	{
		for (const auto& e : *this)
		{
			if (!std::none_of(set->begin(), set->end(), [&e, &knowledge](const Object& ov)
			{
				return e.Match(*ov, knowledge);
			}))
			{
				return false;
			}
		}
		return true;
	}
	return false;
}

Object Set::Cast(const std::type_info& t, const Knowledge& k) const
{
    if (t == typeid(Boolean))
    {
        return boolean(!empty());
    }
    else if (t == typeid(Sequence))
    {
        return Object(); // TODO: copy all members 
    }
    throw CastException<Set>(t);
}

void Set::Append(Object&& value)
{
	if (value)
		insert(std::move(value));
}

void Set::Merge(Set&& other)
{
	while(!other.empty())
		insert(std::move(other.extract(other.begin())));
}

Object set()
{
	return Create<Set>();
}

Object set(Sequence&& array)
{
	return Create<Set>(std::move(array));
}

}
}
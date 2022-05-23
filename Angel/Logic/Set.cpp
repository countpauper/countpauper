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


Set::Set(Set&& other) :
    std::unordered_set<Object>(std::move(other))
{
}

Set::Set(Sequence&& seq)
{
    for (auto& si : seq)
        emplace(std::move(si));
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

Object Set::Match(const Expression& expr) const
{
	if (auto set = dynamic_cast<const Set*>(&expr))
	{
		for (const auto& e : *this)
		{
			if (!std::none_of(set->begin(), set->end(), [&e](const Object& ov)
			{
				return e.Match(*ov);
			}))
			{
                return boolean(false);
			}
		}
		return boolean(true);
	}
	return boolean(false);
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

void Set::Add(Object&& value)
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

Object set(Sequence&& seq)
{
    return Create<Set>(std::move(seq));
}

Object set(Set&& s, Object&& o)
{
    auto result = Create<Set>(std::move(s));
    result.As<Set>()->Add(std::move(o));
    return std::move(result);
}

}
}
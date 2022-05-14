#include "stdafx.h"
#include "Set.h"
#include <algorithm>
#include "Array.h"
#include "Boolean.h"

namespace Angel
{
namespace Logic
{

Set::Set() = default;

Set::Set(Array&& array)
{
	for (auto& e : array)
	{
		emplace(std::move(e));
	}
}

Set::Set(Object&& value)
{
	if (auto array = value.As<Array>())
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

Set::Set(Set&& other) :
	std::unordered_set<Object>(std::move(other))
{
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
    else if (t == typeid(Array))
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

Object set(Array&& array)
{
	return Create<Set>(std::move(array));
}

}
}
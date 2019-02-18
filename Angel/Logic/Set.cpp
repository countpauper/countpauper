#include "stdafx.h"
#include "Set.h"
#include <algorithm>
#include "Array.h"

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

Set::Set(Element&& value)
{
	if (auto array = value.Cast<Array>())
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
	std::unordered_set<Element>(std::move(other))
{
}


bool Set::operator==(const Value& value) const
{
	if (auto set = dynamic_cast<const Set*>(&value))
	{
		return std::operator==(static_cast<const std::unordered_set<Element>&>(*this),
			static_cast<const std::unordered_set<Element>&>(*set));
	}
	return false;
}

bool Set::Match(const Value& value, const Knowledge& knowledge) const
{
	if (auto set = dynamic_cast<const Set*>(&value))
	{
		for (const auto& e : *this)
		{
			if (!std::none_of(set->begin(), set->end(), [&e, &knowledge](const Element& ov)
			{
				return e.Match(ov, knowledge);
			}))
			{
				return false;
			}
		}
		return true;
	}
	return false;
}


void Set::Append(Element&& value)
{
	if (value)
		insert(std::move(value));
}

void Set::Merge(Set&& other)
{
	while(!other.empty())
		insert(std::move(other.extract(other.begin())));
}

Element set()
{
	return Element(std::make_unique<Set>());
}

Element set(Array&& array)
{
	return Element(std::make_unique<Set>(std::move(array)));
}

}
}
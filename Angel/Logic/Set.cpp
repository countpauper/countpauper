#include "Logic/Set.h"
#include "Logic/Object.h"
#include "Logic/Boolean.h"
#include <algorithm>
#include <iostream>
namespace Angel::Logic
{

Set::Set(std::initializer_list<Object> setItems)
{
    for(const auto& item: setItems)
    {
        emplace(std::make_pair(std::move(item),Boolean(false)));
    }
}

// TODO: this is weird now
const Object* Set::Find(const Object& o) const
{
    for(const auto &association : *this)
    {
        if (association.first == o)
            return &association.second;
    }
    return nullptr;
}

Set::operator bool() const
{
    return !empty();
}

bool Set::operator==(const Set& rhs) const
{
    return size() == rhs.size() &&
        std::equal(begin(), end(),
                      rhs.begin());
}

std::size_t Set::Hash() const
{
    std::size_t result = typeid(decltype(*this)).hash_code();
    std::hash<Object> hasher;
    for(const auto& association: *this)
    {
        result ^= hasher(association.first) ^ hasher(association.second);
    }
    return result;
}

std::ostream& operator<<(std::ostream& os, const Set& set)
{
    os << "{";
    bool first = true;
    for(const auto& node: set)
    {
        if (!first)
            os << ",";
        os << node.first << ":" << node.second;
        first = false;
    }

    os << "}";
    return os;
}
    /*
Match Set::Matching(const Expression& expr, const Variables& substitutions) const
{

    Variables substitutions;
	if (auto set = dynamic_cast<const Set*>(&expr))
	{
		for (const auto& e : *this)
		{
			if (!std::none_of(set->begin(), set->end(), [&e](const Object& ov)
			{
				return e.Matching(*ov); // TODO: should accumulate all of the matching variables
			}))
			{
                return NoMatch;
			}
		}
		return Match(substitutions);
	}
    	return NoMatch;
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
*/

}
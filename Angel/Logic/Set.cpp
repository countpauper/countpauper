#include "Logic/Set.h"
#include "Logic/Sequence.h"
#include "Logic/Boolean.h"
#include <algorithm>

namespace Angel::Logic
{

    /*
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

*/

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


/*
Object Set::Copy() const
{
    return Create<Set>(*this);
}

Set::operator bool() const
{
    return !empty();
}
*/

bool Set::operator==(const Set& rhs) const
{
    return size() == rhs.size() &&
        std::equal(begin(), end(),
                      rhs.begin());
}

/*


std::ostream& operator<<(std::ostream& os, const Set& set)
{
    bool first = true;
    os << "{";
    for (const auto& element : set)
    {
        if (first)
            first = false;
        else
            os << " , ";
        os << element;
    }
    os << "}";
    return os;
}

*/

Set::Set(std::initializer_list<Node> setItems)
{
    for(const auto& item: setItems)
    {
        emplace(std::make_pair(std::move(item),Boolean(false)));
    }
}


const Node* Set::Find(const Node& o) const
{
    for(const auto &pair : *this)
    {
        if (pair.first.value == o.value)
            return &pair.second;
    }
    return nullptr;
}

Set::operator bool() const
{
    return !empty();
}

std::size_t Set::Hash() const
{
    std::size_t result = typeid(decltype(*this)).hash_code();
    std::hash<Node> hasher;
    for(const auto& pair: *this)
    {
        result ^= hasher(pair.first) ^ hasher(pair.second);
    }
    return result;
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
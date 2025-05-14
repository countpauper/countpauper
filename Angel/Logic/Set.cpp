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
        emplace(std::make_pair(std::move(item),Boolean(true)));
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

Match Set::Matches(const Object& o, const Variables& variables) const
{
    const Set* set = std::get_if<Set>(&o);
    if (!set)
        return NoMatch;
        
    // TODO: implement, but how and what behavior 
    // {ginny, max} should match {max, ginny}
    // {ginny, $X, gizmo } should match {max, ginny, $Y} with $X = max & $Y = gizmo 
    // should be the same size at least, barring some sort of |Tail syntax 
    // so all items should be attempted to match with another and then that one should be out
    if (size()!=set->size())
        return NoMatch; // TODO: head|tail matching or whatever could happen here
    return NoMatch;
}

Object Set::Compute(const Knowledge& knowledge, const Variables& substitutions) const
{
    Set result;
    for(const std::pair<Object,Object> item: *this)
    {
        // NB right side computed get lost here if the first side is now duplicated 
        // eg {X+1: cheese, X*2:pickles} with X=1 becomes {2:cheese} (or pickles perhaps)
        // this might be what is desired or maybe some expression makes more sense 
        // {2: cheese | pickles}. 
        // For cases where these are sets, which are true 2: true|true is still 2:true 
        // for cases where these are clauses eg legs(X+1): max legs(X*2): ginny, 
        // this disjunction also seems to make sense legs(2): max | ginny, if we are 
        // hypothesizing that both have 2 legs 
        result.emplace(item.first.Compute(knowledge, substitutions),
            item.second.Compute(knowledge,substitutions));
    }
    return result;
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
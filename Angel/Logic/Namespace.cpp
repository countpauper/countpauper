#include <algorithm>
#include "Logic/Namespace.h"
#include "Logic/Disjunction.h"
#include "Logic/Knowledge.h"
#include "Logic/Set.h"

namespace Angel
{
namespace Logic
{

Namespace::Namespace(const Id& id) :
	id(id)
{
}

size_t Namespace::Add(Clause&& c)
{
    if (Contains(c))
        return 0;
    contents.emplace_back(std::move(c));
    return 1; 
}

Object Namespace::Match(const Expression& e) const
{
    Disjunction result;
	for (auto& c : contents)
	{
        Object match = c.Match(e);
        if (match!=boolean(false))  // optimization, but only works because empty result is also false
            result.Add(std::move(match));
	}

    return result.Simplify();
}

bool Namespace::Contains(const Expression& e) const
{
    for (auto& c : contents)
	{
        if (c == e) {
            return true;
        }
    }
    return false;
}

size_t Namespace::Clauses() const
{
	return contents.size();
}

}
}
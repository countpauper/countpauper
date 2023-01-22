#include "stdafx.h"
#include <algorithm>
#include "Namespace.h"
#include "Disjunction.h"
#include "Knowledge.h"
#include "Set.h"

namespace Angel
{
namespace Logic
{

Namespace::Namespace(const Id& id) :
	id(id)
{
}

void Namespace::Add(Clause&& c)
{
    if (!Contains(c))
        contents.emplace_back(std::move(c));
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
    auto match = Match(e);
    return match.Trivial();
}



size_t Namespace::Clauses() const
{
	return contents.size();
}

}
}
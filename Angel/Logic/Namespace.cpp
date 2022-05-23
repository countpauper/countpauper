#include "stdafx.h"
#include <algorithm>
#include "Namespace.h"
#include "Disjunction.h"
#include "Knowledge.h"

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
    Set result;
	for (auto& c : contents)
	{
        Object match = c.Match(e);
        if (match!=boolean(false))  // optimization, but only works because empty result is also false
            result.Add(std::move(match));
	}

    if (result.empty())
    {
        return boolean(false);
    }
    else if (result.size() == 1)
    {
        return Object(std::move(*result.begin()));
    }
    else
    {
        return Create<Disjunction>(std::move(result));
    }
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
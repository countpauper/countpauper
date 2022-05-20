#include "stdafx.h"
#include <algorithm>
#include "Namespace.h"

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

bool Namespace::Match(const Expression& e, const Knowledge& knowledge) const
{
	for (auto& c : contents)
	{
		if (c.Match(e, knowledge))
			return true;
	}
	return false;
}

bool Namespace::Contains(const Clause& c) const
{
    return std::find(contents.begin(), contents.end(), c) != contents.end();
}

bool Namespace::Contains(const Object& e) const
{
    if (Clause* c = e.As<Clause>())
    {
        return Contains(*c);
    }
    else
    {
        return false;
    }
}


size_t Namespace::Clauses() const
{
	return contents.size();
}

}
}
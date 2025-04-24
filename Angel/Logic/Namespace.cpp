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



Matches Namespace::FindMatches(const Expression& e) const
{
    Matches result; 
	for (const auto& c : contents)
	{
        Match m = c.Matching(e);
        if (m)
            result.emplace(std::make_pair(&c, std::move(*m)));
	}

    return result;
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
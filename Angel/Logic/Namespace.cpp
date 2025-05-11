#include <algorithm>
#include "Logic/Namespace.h"
#include "Logic/Disjunction.h"
#include "Logic/Knowledge.h"
#include "Logic/Set.h"

namespace Angel
{
namespace Logic
{

    /*
    Namespace::Namespace(const Id& id) :
	id(id)
{
}

size_t Namespace::Add(Object&& o)
{
    if (Contains(o))
        return 0;
    emplace_back(std::move(o));
    return 1; 
}



Matches Namespace::FindMatches(const Expression& e) const
{
    Matches result; 
	for (const auto& o : *this)
	{
        Match m = o->Matching(e, {});
        if (m)
            result.emplace(std::make_pair(&o, std::move(*m)));
	}
    return result;
}
*/

}
}
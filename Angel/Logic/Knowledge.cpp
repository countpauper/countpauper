#include "Logic/Knowledge.h"
#include "Logic/Set.h"
#include "Logic/Association.h"
#include <variant>
#include <cassert>

namespace Angel::Logic
{

Knowledge::Knowledge() :
	root()
{
}

size_t Knowledge::Know(Predicate&& key, Object&& expression)
{
    auto insert = root.emplace(std::make_pair(std::move(key), std::move(expression)));
    if (insert.second) {
        return 1;
    } else {
        return 0;
    }
}

Element Knowledge::Compute(const Object& expression) const
{
    Variables vars;
    return expression.Compute(*this, vars);
}

Match Knowledge::Matches(const Predicate& query) const
{
    for(const auto& association: root)
    {
        if (const auto* predicate = std::get_if<Predicate>(&association.first))
        {
            auto match = predicate->Matches(query);
            if ((match) && (association.second.Compute(*this, *match)))
                return match;                
        }
        else 
            assert(false && "Only predicate keys are allowed in knowledge");
    } 
    return NoMatch;
}


bool Knowledge::Knows(const Object& o) const
{
    return root.Find(o) != nullptr;
}

size_t Knowledge::size() const
{
	return root.size();
}

bool Knowledge::empty() const 
{
    return root.empty();
}

}
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

Object Knowledge::Compute(const Object& expression) const
{
    Variables vars;
    return expression.Compute(*this, vars);
}

Set Knowledge::Matches(const Predicate& query) const
{
    Set result;
    for(const auto& association: root)
    {
        if (const auto* predicate = std::get_if<Predicate>(&association.first))
        {
            auto match = predicate->Matches(query);
            if (match) 
            {
                result.emplace(association.first, association.second.Compute(*this, *match));
            }
        }
        else 
            assert(false && "Only predicate keys are allowed in knowledge");
    } 
    return result;
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
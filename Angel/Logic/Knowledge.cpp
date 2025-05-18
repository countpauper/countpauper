#include "Logic/Knowledge.h"
#include "Logic/Set.h"
#include "Logic/Association.h"
#include <variant>
#include <cassert>

namespace Angel::Logic
{
\
Knowledge::Knowledge() :
	root()
{
    AddDefaults();
}

size_t Knowledge::Know(Predicate&& key, Expression&& expression)
{
    Association association{std::move(key), std::move(expression)};
    auto it = std::find(root.begin(), root.end(), association);
    if (it!=root.end())
        return 0;
    root.emplace_back(std::move(association));
    return 1;
}

Object Knowledge::Infer(const Expression& expression) const
{
    Variables vars;
    return expression.Infer(*this, vars);
}

Bag Knowledge::Matches(const Predicate& query) const
{
    Bag result;
    std::size_t bestMatch = std::numeric_limits<std::size_t>::max();
    for(const auto& item: root)
    {
        const Association& association = item.Cast<Association>();
        Expression lhs = association.Left();
        if (const auto* predicate = std::get_if<Predicate>(&lhs))
        {
            auto match = predicate->Matches(query, {});
            if (match)
            {   // Matches with least substitutions take precedence. 
                // See the Occam's razor section in language design
                if (match->size() > bestMatch)
                    continue;
                if (match->size() < bestMatch)
                    result.clear();
                result.emplace_back(Association{std::move(lhs), association.Right().Infer(*this, *match)});
            }
        }
        else 
            assert(false && "Only predicate keys are allowed in knowledge");
    } 
    return result;
}


bool Knowledge::Knows(const Expression& e) const
{
    return *root.Get(e).Cast<Boolean>();
}

const Bag& Knowledge::Root() const
{
    return root;
}

void Knowledge::AddDefaults()
{
    Know(Predicate("true"), Boolean(true));
    Know(Predicate("false"), Boolean(false));
}

}
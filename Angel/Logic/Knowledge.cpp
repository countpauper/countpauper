#include "Logic/Knowledge.h"
#include "Logic/Association.h"
#include "Logic/Disjunction.h"
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

Expression Knowledge::Matches(const Predicate& query) const
{
    std::vector<Expression> result;
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
                {
                    bestMatch = match->size();
                    result.clear();
                }
                result.emplace_back( association.Right().Infer(*this, *match));
            }
        }
        else 
            assert(false && "Only predicate keys are allowed in knowledge");
    } 
    if (result.empty())
        return Boolean(false);
    else if (result.size()==1)
        return std::move(result.front());
    else
        return Disjunction(std::move(result));
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
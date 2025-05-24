#include "Logic/Knowledge.h"
#include "Logic/Expression.h"
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


Knowledge::Knowledge(std::initializer_list<Expression> knowledge)
{
    for(auto item: knowledge)
    {
        Know(std::move(item));
    }
}

size_t Knowledge::Know(Expression&& clause)
{
    if (auto* predicate = clause.GetIf<Predicate>())
    {
        return Know(Association{std::move(*predicate), Boolean(true)});
    }
    else if (auto* association = clause.GetIf<Association>())
    {
        auto it = std::find(root.begin(), root.end(), clause);
        if (it!=root.end())
            return 0;
        if (!association->Left().Is<Predicate>())
        {
            throw std::invalid_argument("Clauses must have a predicate on the left hand side");
        }
        root.emplace_back(std::move(clause));
        return 1;
    }
    else 
    {
        throw std::invalid_argument("Clauses must be an association of a predicate and an optional antecedent");
    }

}

Expression Knowledge::Infer(const Expression& expression) const
{
    Variables vars;
    return expression.Infer(*this, vars);
}


Bag Knowledge::Matches(const Predicate& query) const
{
    Bag hypotheses;
    std::size_t bestMatch = std::numeric_limits<std::size_t>::max();
    for(const auto& item: root)
    {
        const Association& association = item.Cast<Association>();
        Expression lhs = association.Left();
        if (const auto* predicate = lhs.GetIf<Predicate>())
        {
            Conjunction hypothesis;
            const auto& match = predicate->Matches(query, {});
            if (match == Boolean(false))
                continue;
            if (match == Boolean(true))
            {
                
            }
            else if (const auto* conj = match.GetIf<Conjunction>())
            {
                hypothesis = *conj;
            }

            if (hypothesis.size() > bestMatch)
                continue;
            if (hypothesis.size() < bestMatch)
            {
                bestMatch = hypothesis.size();
                hypotheses = Bag(); // clear worse hypotheses
            }

            auto valueResult =  association.Right().Simplify();
            if (valueResult==Boolean(false))
                continue;
            if (hypothesis.empty())
                hypotheses.emplace_back(std::move(valueResult));
            else
                hypotheses.emplace_back(Association(std::move(valueResult), std::move(hypothesis)));
        }
        else 
            assert(false && "Only predicate keys are allowed in knowledge");
    } 
    return hypotheses;
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
    Know(Predicate("true"));
    Know(Association(Predicate("false"), Boolean(false)));
}

}
#include "Logic/Knowledge.h"
#include "Logic/Expression.h"
#include "Logic/Association.h"
#include "Logic/Disjunction.h"
#include "Logic/Trace.h"
#include <variant>
#include <cassert>

namespace Angel::Logic
{

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
        return Know(Association{std::move(*predicate), True});
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

size_t Knowledge::Forget(const Predicate& match)
{
    return std::erase_if(root, [&match](const Expression& item)
    {
        const Association& clause = item.Get<Association>();
        return clause.Left().Matches(match, {}).Simplify() != False;
    });
}

Expression Knowledge::Infer(const Expression& expression) const
{
    Hypothesis hypothesis;
    Trace trace;
    return expression.Infer(*this, hypothesis, trace).Simplify();
}

Hypothesis GetAntecedent(Expression&& match)
{
    if (match == True)
    {
        return Hypothesis();            
    }
    
    if (const auto* conj = match.GetIf<Conjunction>())
    {
        return std::move(*conj);
    }
    
    assert(match.IsComparison());   // unexpected match antecdent;
    return Hypothesis{std::move(match)};
}
Bag Knowledge::Matches(const Predicate& query) const
{
    Bag hypotheses;
    std::size_t bestMatch = std::numeric_limits<std::size_t>::max();
    for(const auto& item: root)
    {
        const Association& clause = item.Get<Association>();
        Expression lhs = clause.Left();
        const auto& predicate = lhs.Get<Predicate>();
        auto match = predicate.Matches(query, {});
        if (match.Simplify() == False)
            continue;
        Hypothesis hypothesis = GetAntecedent(std::move(match));
        auto assumptions = hypothesis.Assumptions().size();
        if (assumptions > bestMatch)
            continue;
        if (hypothesis.size() < bestMatch)
        {
            bestMatch = assumptions;
            hypotheses = Bag(); // clear worse hypotheses
        }

        auto valueResult =  clause.Right().Simplify();
        if (valueResult==False)
            continue;
        if (hypothesis.empty())
            hypotheses.emplace_back(std::move(valueResult));
        else
            hypotheses.emplace_back(Association(std::move(valueResult), std::move(hypothesis)));
    } 
    return hypotheses;
}


Expression Knowledge::Knows(const Expression& e) const
{
    auto found = root.Get(e);
    if (const auto* i = found.GetIf<Integer>())
        return Boolean(*i);
    else 
        return found;

}

const Bag& Knowledge::Root() const
{
    return root;
}

Knowledge& Knowledge::Lock() const
{
    return const_cast<Knowledge&>(*this);
}


void Knowledge::AddDefaults()
{
    Know(Predicate("true"));
    Know(Association(Predicate("false"), False));
}


std::ostream& operator<<(std::ostream& os, const Knowledge& knowledge)
{
    for(const auto& clause: knowledge.Root())
        os << clause << std::endl;
    return os;
}

}
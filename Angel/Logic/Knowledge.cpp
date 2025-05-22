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
    if (auto* predicate = std::get_if<Predicate>(&clause))
    {
        return Know(Association{std::move(*predicate), Boolean(true)});
    }
    else if (auto* association = std::get_if<Association>(&clause))
    {
        auto it = std::find(root.begin(), root.end(), clause);
        if (it!=root.end())
            return 0;
        if (!std::get_if<Predicate>(&association->Left()))
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

Variables LeftVariablesOnly(Variables& substitutions)
{
    Variables result;
    for(const auto& sub : substitutions)
    {
        if (const auto* equation = std::get_if<Equation>(&sub))
        {
            if (std::get_if<Variable>(&equation->front()))
            {
                result.emplace_back(std::move(*equation));
            }
        }
    }
    return result;
}

Expression Hypothesis(Expression value, Variables& substitutions)
{   // variables in the query end up in the substitutions as $Var = value 
    // variables in the axiom that are matched to the query as value = $var 
    // this is done through the `reverse` option in Variable::Matches, which is used 
    // while matching values to variables instead of matching variables to values.
    // The query's variables are in scope of the query, so those are "returned".
    // The axiom's variables are in scope of the axiom so they are omitted from the hypothesis.
    auto left = LeftVariablesOnly(substitutions);
    if (left.empty())
        return std::move(value);
    if (value == Boolean(false))
        return std::move(value);
    if (left.size()==1)
        return Association{std::move(value), std::move(left.front())};
    else
        return Association{std::move(value), std::move(left)};
}

Expression SimplifyHypotheses(Set& hypotheses)
{
    if (hypotheses.size()==0)
        return Boolean(false);
    else if (hypotheses.size()==1)
    {
        const auto& single = *hypotheses.begin();
        return single;
    }
    else
        return std::move(hypotheses);
}


Expression Knowledge::Matches(const Predicate& query) const
{
    Set hypotheses;
    std::size_t bestMatch = std::numeric_limits<std::size_t>::max();
    for(const auto& item: root)
    {
        const Association& association = item.Cast<Association>();
        Expression lhs = association.Left();
        if (const auto* predicate = std::get_if<Predicate>(&lhs))
        {
            Conjunction hypothesis;
            const auto& match = predicate->Matches(query, {});
            if (match == Boolean(false))
                continue;
            if (match == Boolean(true))
            {
                
            }
            else if (const auto* conj = std::get_if<Conjunction>(&match))
            {
                hypothesis = *conj;
            }

            if (hypothesis.size() > bestMatch)
                continue;
            if (hypothesis.size() < bestMatch)
            {
                bestMatch = hypothesis.size();
                hypotheses = Set(); // clear worse hypotheses
            }
            auto valueResult = association.Right().Infer(*this, hypothesis);
            if (valueResult==Boolean(false))
                continue;
            assert(!hypotheses.Get(valueResult)); // disjunction extension not yet implemented
            
            hypotheses.Add(Hypothesis(valueResult, hypothesis));
        }
        else 
            assert(false && "Only predicate keys are allowed in knowledge");
    } 
    return SimplifyHypotheses(hypotheses);
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
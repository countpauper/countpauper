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

Variables LeftVariablesOnly(Variables& substitutions)
{
    Variables result;
    for(const auto& sub : substitutions)
    {
        if (const auto* equation = sub.GetIf<Equation>())
        {
            if (equation->front().Is<Variable>())
            {
                result.emplace_back(std::move(*equation));
            }
        }
    }
    return result;
}

Expression Hypothesis(Expression value, Expression& current, Variables& substitutions)
{   // variables in the query end up in the substitutions as $Var = value 
    // variables in the axiom that are matched to the query as value = $var 
    // this is done through the `reverse` option in Variable::Matches, which is used 
    // while matching values to variables instead of matching variables to values.
    // The query's variables are in scope of the query, so those are "returned".
    // The axiom's variables are in scope of the axiom so they are omitted from the hypothesis

    // The current hypothesis is added as a disjunction
    if (value == Boolean(false))
        if (current)    
            return std::move(current);
        else
            return std::move(value);  
    auto left = LeftVariablesOnly(substitutions);
    // TODO: perhaps should compare with current (if disjunction all) conjunctions. 
    // ockam's razor should discard them or left depending on size
    return Association{std::move(value), Disjunction{std::move(current), std::move(left)}}.Simplify();
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
                hypotheses = Set(); // clear worse hypotheses
            }

            auto valueResult = association.Right().Substitute(hypothesis);
            valueResult = valueResult.Simplify();
            if (valueResult==Boolean(false))
                continue;
            auto current = hypotheses.Pop(valueResult);
            hypotheses.Add(Hypothesis(valueResult, current, hypothesis));
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
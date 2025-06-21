#include <algorithm>
#include "Logic/Predicate.h"
#include "Logic/Boolean.h"
#include "Logic/Knowledge.h"
#include "Logic/Expression.h"
#include "Logic/Trace.h"
#include <cassert>

namespace Angel::Logic
{

Predicate::Predicate(Predicate&& o) :
	id()
{
	std::swap(id, o.id);
	std::swap(arguments, o.arguments);
}

Predicate& Predicate::operator=(const Predicate& o)
{
	id = o.id;
	arguments = o.arguments;
	return *this;
}

Predicate::Predicate(const Id& id, List&& arguments) 
	: id(id)
	, arguments(std::move(arguments))
{
	if (!id)
		throw std::invalid_argument("Predicate identity can not be empty.");
}

Predicate::Predicate(const std::string& tag, List&& arguments) 
	: Predicate(Id(tag), std::move(arguments))
{
}

Predicate::operator bool() const
{
    return bool(id);
}

bool Predicate::operator==(const Predicate& rhs) const
{
	return id == rhs.id && arguments == rhs.arguments;
}

std::size_t Predicate::Valence() const 
{
	return arguments.size();
}

std::size_t Predicate::Hash() const
{
    return id.Hash() ^ arguments.Hash();
}


Expression Predicate::Simplify() const
{
    return *this;   
}

Set Predicate::Assumptions() const
{
	return arguments.Assumptions();
}

Expression Predicate::Matches(const Expression& inferred, const Hypothesis& hypothesis) const
{
	const auto* predicate = inferred.GetIf<Predicate>();
	if (!predicate)
		return Boolean(false);
	
	if (id != predicate->id) // Variable predicate names not (yet) supported
		return Boolean(false);
	return arguments.Simplify().Matches(predicate->arguments.Simplify(), hypothesis);
}

Predicate Predicate::Substitute(const Hypothesis& hypothesis) const
{
    return Predicate(id, arguments.Substitute(hypothesis));
}


bool IsQueryHypothesis(const Expression& e)
{
	return std::visit(overloaded_visit{
        []<Logic::is_ordering C>(const C& comparison)
		{
			return comparison.HasLeftAssumption();
		}, 
		[](const auto&) { return false; }   
		}, e);

}

// When a query matches with a predicate there can be four cases per argument
// 1. both are constant (and match)
// 2. the query has a variable. The caller is querying what it was matched with for the hypothesis to be true. It's like a return value  
// 3. The predicate has a variable. A generic case. The antecedent uses it to determine if it is true for the iput.
// 4. Both the query and the predicate have a variable. TBD what this would do
// Because the caller of the query provided the hypothesis in case 3, it does nnot need to be returned. 
// during matches these hypothesis end up with the variable on the right input = $ARG and are filtered out here 
// In case 2 the hypothesis has the variable on the left $ARG<input. These are kept 
Hypothesis QueryHypothesisOnly(const Hypothesis& hypothesis)
{
	// TODO: input does not need to be const. Would be more performant if erase in place
	// on the other hand keeping output hypothesis and infer context separate might
	// remove the whole left/right hack
    Hypothesis result;
    for(const auto& sub : hypothesis)
    {
		if (IsQueryHypothesis(sub))
		{
			result.push_back(sub);
		}
    }
    return result;
}

Expression ExtendResult(Expression&& value, Expression&& current, Hypothesis&& hypothesis)
{   // variables in the query end up in the hypothesis as $Var = value 
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
    auto left = QueryHypothesisOnly(hypothesis);
    // TODO: perhaps should compare with current (if disjunction all) conjunctions. 
    // ockam's razor should discard them or left depending on size
    return Association{std::move(value), Disjunction{std::move(current), std::move(left)}}.Simplify();
}

Expression Predicate::Infer(const Knowledge& knowledge, const Hypothesis& originalHypothesis, Trace& parentTrace) const
{
	Predicate simple(id, arguments.Substitute(originalHypothesis).Simplify());
	Trace trace(parentTrace, simple);
	Set result;
	auto hypotheses = knowledge.Matches(simple);

	for(auto option: hypotheses)
	{
		Expression value;
		Hypothesis conditions;
		if (auto* association = option.GetIf<Association>())
		{
			conditions = association->Right().Get<Conjunction>();
			value = association->Left().Infer(knowledge, conditions, trace);
		}
		else 
		{ 
			Hypothesis newHypothesis;
			value = option.Infer(knowledge, newHypothesis, trace);
		}
		Expression current = result.Pop(value);
		result.Add(ExtendResult(std::move(value), std::move(current), std::move(conditions)));
	}
	if (result.empty())
		return Boolean(false);
	if (result.size()==1)
		return *result.begin();
	return result;
}

std::ostream& operator<<(std::ostream& os, const Predicate& predicate)
{
	if (predicate.arguments)
	{
		os << predicate.id << "(";
		bool first = true;
		for(const auto& arg: predicate.arguments)
		{
			if (!first)
				os << ",";
			os << arg;
			first = false;
		}
		os << ")";
	}
	else
	{
		os << predicate.id;
	}
    return os;
}

}
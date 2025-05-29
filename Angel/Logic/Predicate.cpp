#include <algorithm>
#include "Logic/Predicate.h"
#include "Logic/Boolean.h"
#include "Logic/Knowledge.h"
#include "Logic/Expression.h"
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

std::size_t Predicate::Hash() const
{
    return id.Hash() ^ arguments.Hash();
}


Expression Predicate::Simplify() const
{
    return *this;   
}


Expression Predicate::Matches(const Expression& inferred, const Substitutions& substitutions) const
{
	const auto* predicate = inferred.GetIf<Predicate>();
	if (!predicate)
		return Boolean(false);
	
	if (id != predicate->id) // Variable predicate names not (yet) supported
		return Boolean(false);
	return arguments.Simplify().Matches(predicate->arguments.Simplify(), substitutions);
}

Predicate Predicate::Substitute(const Substitutions& substitutions) const
{
    return Predicate(id, arguments.Substitute(substitutions));
}

Substitutions LeftVariablesOnly(Substitutions& substitutions)
{
    Substitutions result;
    for(const auto& sub : substitutions)
    {
        const auto& equation = sub.Get<Equation>();
		if (equation.front().Is<Variable>())
		{
			result.emplace_back(std::move(equation));
		}
		else if (equation.front().Is<Tuple>())
		{
			assert(false); // tuples should not be in the hypothesis
		}
    }
    return result;
}

Expression Hypothesis(Expression&& value, Expression&& current, Substitutions&& substitutions)
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

Expression Predicate::Infer(const Knowledge& knowledge, const Substitutions& substitutions) const
{
	Predicate simple(id, std::get<List>(arguments.Substitute(substitutions).Simplify()));
	Set result;
	auto hypotheses = knowledge.Matches(simple);

	for(auto hypothesis: hypotheses)
	{
		Expression value;
		Substitutions conditions;
		if (auto* association = hypothesis.GetIf<Association>())
		{
			conditions = association->Right().Get<Conjunction>();
			value = association->Left().Infer(knowledge, conditions);
		}
		else 
		{
			value = hypothesis.Infer(knowledge, {});
		}
		Expression current = result.Pop(value);
		result.Add(Hypothesis(std::move(value), std::move(current), std::move(conditions)));
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
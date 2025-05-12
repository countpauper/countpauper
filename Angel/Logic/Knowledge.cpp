#include "Logic/Knowledge.h"
#include "Logic/Set.h"
#include "Logic/Clause.h"
#include <variant>
#include <cassert>

namespace Angel::Logic
{

Knowledge::Knowledge() :
	root()
{
}

size_t Knowledge::Know(Object&& o)
{
    if (auto* predicate = std::get_if<Predicate>(&o))
    {
        auto insert = root.emplace(std::make_pair(std::move(o), Object(Boolean(true))));
        if (insert.second) {
            return 1;
        } else {
            return 0;
        }
    }
    // TODO pairs go in as a pair, clauses and namespaces only, so left needs to be a predicate or axiom 
    else 
    {
        throw std::invalid_argument("Only clauses and namespaces can be Known.");
    }
}

Object Knowledge::Compute(const Object& expression) const
{
    Variables vars;
    return expression.Compute(*this, vars);
}

Object Knowledge::Query(const Predicate& queryPredicate) const
{
    for(const auto& pair: root)
    {
        if (const auto* predicate = std::get_if<Predicate>(&pair.first))
        {
            auto match = predicate->Matches(queryPredicate);
            if (match)
            {
                // TODO: compute with vars
                return pair.second.Compute(*this, *match);
            }
        }
        else 
        {
            assert(false && "Only predicate keys are allowed in knowledge");
        }
    }
    return Boolean(false);
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
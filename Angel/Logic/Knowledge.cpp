#include "Logic/Knowledge.h"
#include "Logic/Set.h"
#include "Logic/Clause.h"
#include <variant>

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
        auto insert = root.emplace(std::make_pair(Node(std::move(o)), Node(Boolean(true))));
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

Object Knowledge::Query(const Object& o, const Variables& substitutions) const
{
    if (const auto queryPredicate = TryCast<Predicate>(o))
    {
        for(const auto& pair: root)
        {
            if (const auto* predicate = std::get_if<Predicate>(&pair.first.value))
            {
                Variables vars;
                auto match = predicate->Matches(*queryPredicate, vars);
                if (match)
                {
                    // TODO: compute with vars
                    return pair.second.value;
                }
            }
        }
        return Boolean(false);
    }
    return o;   // TODO: compute 
}

bool Knowledge::Knows(const Object& o) const
{
    return root.Find(Node{o}) != nullptr;

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
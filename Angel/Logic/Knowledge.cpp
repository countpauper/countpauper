#include "Logic/Knowledge.h"
#include "Logic/Set.h"
#include "Logic/Clause.h"

namespace Angel
{
namespace Logic
{

Knowledge::Knowledge() :
	root(Id(""))
{
}

size_t Knowledge::Know(Object&& o)
{
    if (const auto* pred = o.As<Predicate>())
    {
        return root.Add(std::move(o));
    }
    else if (const auto* clause = o.As<Clause>())
    {
        return root.Add(std::move(o));
    }
    else 
    {
        throw std::invalid_argument("Only clauses and predicates can be known");
    }
}

Object Knowledge::Query(const Object& o, const Variables& substitutions) const
{
    return Query(*o, substitutions);
}

Object Knowledge::Query(const Expression& e, const Variables& substitutions) const
{
    return e.Infer(*this, substitutions);
}

Object Knowledge::Match(const Expression& e) const
{
    for(const auto& match: root.FindMatches(e))
    {
        auto condition = (*match.first)->Condition();
        if (!condition)
            return boolean(true);
        auto result = condition->Infer(*this, match.second);
        if (result) 
        {
            return result;
        }
    } 
    return boolean(false);
}

bool Knowledge::Knows(const Object& o) const
{
    return  root.Contains(o);
}

size_t Knowledge::Clauses() const
{
	return root.Size();
}

}
}
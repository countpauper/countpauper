#include "stdafx.h"
#include "Knowledge.h"
#include "Set.h"
#include "Clause.h"

namespace Angel
{
namespace Logic
{

Knowledge::Knowledge() :
	root(Id(L""))
{
}

void Knowledge::Know(Object&& e)
{
    if (auto* c = e.As<Clause>())
    {
        root.Add(std::move(*c));
    }
    else if (auto* predicate = e.As<Predicate>())
    {
        root.Add(Clause(std::move(*predicate)));
    }
    else
    {
        throw std::invalid_argument(std::string("Only Clauses can be known, not ") + typeid(*e).name());
    }
}

bool Knowledge::Query(const Object& o) const
{
    if (o.Trivial())
        return true;
    return Query(*o);
}

bool Knowledge::Query(const Expression& e) const
{
	return root.Match(e, *this);
}

bool Knowledge::Knows(const Object& e) const
{
    // TODO: contains should just match predicates (without computing them) and check if the returned matches (could be a disjunction or sequence if true) is a boolean false
	return root.Contains(e);
}

size_t Knowledge::Clauses() const
{
	return root.Clauses();
}

}
}
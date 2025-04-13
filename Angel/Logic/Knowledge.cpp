#include "Knowledge.h"
#include "Set.h"
#include "Clause.h"

namespace Angel
{
namespace Logic
{

Knowledge::Knowledge() :
	root(Id(""))
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

Object Knowledge::Query(const Object& o) const
{
    return Query(*o);
}

Object Knowledge::Query(const Expression& e) const
{
    return e.Compute(*this);
}

Object Knowledge::Match(const Expression& e) const
{
    return root.Match(e);
}

bool Knowledge::Knows(const Object& e) const
{
    return root.Contains(*e);
}

size_t Knowledge::Clauses() const
{
	return root.Clauses();
}

}
}
#include "stdafx.h"
#include "Knowledge.h"
#include "Set.h"

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
	root.Add(std::move(e));
}

bool Knowledge::Query(const Object& e) const
{
	if (e.Trivial())
		return true;
	return root.Match(e, *this);
}

bool Knowledge::Knows(const Object& e) const
{
	return root.Contains(e);
}


size_t Knowledge::Clauses() const
{
	return root.Clauses();
}

}
}
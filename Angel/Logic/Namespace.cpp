#include "stdafx.h"
#include <algorithm>
#include "Namespace.h"

namespace Angel
{
namespace Logic
{

Namespace::Namespace(const Id& id) :
	id(id)
{
}

void Namespace::Add(Object&& e)
{
	contents.insert(std::move(e));
}

bool Namespace::Match(const Object& e, const Knowledge& knowledge) const
{
	for (auto& c : contents)
	{
		if (c.Match(e, knowledge))
			return true;
	}
	return false;
}

bool Namespace::Contains(const Object& e) const
{
	return std::find(contents.begin(), contents.end(), e)!=contents.end();
}


size_t Namespace::Clauses() const
{
	return contents.size();
}

}
}
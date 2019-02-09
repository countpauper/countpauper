#include "stdafx.h"
#include "Namespace.h"

namespace Angel
{
namespace Logic
{

Namespace::Namespace(const Id& id) :
	id(id)
{
}

void Namespace::Add(Element&& e)
{
	contents.emplace_back(std::move(e));
}

bool Namespace::Query(const Element& e) const
{
	for (auto& c : contents)
	{
		if (c == e)
			return true;
	}
	return false;
}

}
}
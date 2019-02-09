#include "stdafx.h"
#include "Knowledge.h"
#include "Boolean.h"

namespace Angel
{
namespace Logic
{

Knowledge::Knowledge() :
	root(Id(L""))
{
}

void Knowledge::Know(Element&& e)
{
	root.Add(std::move(e));
}

bool Knowledge::Query(const Element& e) const
{
	if (boolean(true).Match(e))
		return true;
	return root.Match(e);
}

}
}
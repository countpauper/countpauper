#include "stdafx.h"
#include "Knowledge.h"

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
	return root.Query(e);
}

}
}
#include "stdafx.h"
#include "Id.h"

namespace Angel
{
namespace Logic
{

Id::Id(const std::wstring& name) :
	name(name)
{
}

bool Id::operator==(const Id& id) const
{
	return name == id.name;
}


}
}
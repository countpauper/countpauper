#include "stdafx.h"
#include "Item.h"

namespace Angel
{
namespace Logic
{


bool Item::Match(const Item& other, const Knowledge&) const
{
	return operator==(other);
}



}
}
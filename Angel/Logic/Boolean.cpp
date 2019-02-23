#include "stdafx.h"
#include "Boolean.h"

namespace Angel
{
namespace Logic
{

Boolean::Boolean(bool v) :
	truth(v)
{
}

bool Boolean::operator==(const Item& other) const
{
	if (auto boolean = dynamic_cast<const Boolean*>(&other))
	{
		return truth == boolean->truth;
	}
	return false;
}

Object boolean(bool v)
{
	return Object(std::make_unique<Boolean>(v));
}


}
}
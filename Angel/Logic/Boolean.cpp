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

bool Boolean::operator==(const Value& other) const
{
	if (auto boolean = dynamic_cast<const Boolean*>(&other))
	{
		return truth == boolean->truth;
	}
	return false;
}

Element boolean(bool v)
{
	return Element(std::make_unique<Boolean>(v));
}


}
}
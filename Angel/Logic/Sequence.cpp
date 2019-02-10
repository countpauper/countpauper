#include "stdafx.h"
#include "Sequence.h"

namespace Angel
{
namespace Logic
{

Sequence::Sequence() 
{
}

bool Sequence::operator==(const Value& value) const
{
	if (auto sequence= dynamic_cast<const Sequence*>(&value))
	{
		return true;	// TODO: check all elements
	}
	return false;
}

Element sequence()
{
	return Element(std::make_unique<Sequence>());
}


}
}
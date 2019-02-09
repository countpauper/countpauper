#include "stdafx.h"
#include "Element.h"

namespace Angel
{
namespace Logic
{


Element::Element(Element&& other) :
	value(std::move(other.value))
{
}

Element& Element::operator=(Element&& other)
{
	value = std::move(other.value);
	return *this;
}


bool Element::operator==(const Element& other) const
{
	return *value == *other.value;
}


}
}
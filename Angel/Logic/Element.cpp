#include "stdafx.h"
#include "Element.h"

namespace Angel
{
namespace Logic
{

bool Value::Match(const Value& other) const
{
	return operator==(other);
}

Element::Element(Element&& other) :
	value(std::move(other.value))
{
}

Element& Element::operator=(Element&& other)
{
	value = std::move(other.value);
	return *this;
}

Element::operator bool() const
{
	return value.get();
}

bool Element::operator==(const Element& other) const
{
	return *value == *other.value;
}

bool Element::Match(const Element& other) const
{
	return value->Match(*other.value);
}


}
}
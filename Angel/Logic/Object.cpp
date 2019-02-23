#include "stdafx.h"
#include "Object.h"
#include "Boolean.h"

namespace Angel
{
namespace Logic
{

Object::Object(Object&& other) :
	value(std::move(other.value))
{
}

Object& Object::operator=(Object&& other)
{
	value = std::move(other.value);
	return *this;
}

Object::operator bool() const
{
	return value.get();
}

bool Object::Trivial() const
{
	return operator==(boolean(true));
}


bool Object::operator==(const Object& other) const
{
	return *value == *other.value;
}

bool Object::Match(const Object& other, const Knowledge& knowledge) const
{
	return value->Match(*other.value, knowledge);
}

size_t Object::Hash() const
{
	return reinterpret_cast<size_t>(value.get());
}



}
}
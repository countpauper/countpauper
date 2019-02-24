#include "stdafx.h"
#include "Object.h"
#include "Boolean.h"
#include "Integer.h"
#include "Id.h"

namespace Angel
{
namespace Logic
{

Object::Object(const std::wstring& tag)
{
	if (tag.empty())
	{
		return;
	}
	else if (auto boolean = Boolean::Parse(tag))
	{
		item = std::make_unique<Boolean>(*boolean);
	}
	else if (auto integer = Integer::Parse(tag))
	{
		item = std::make_unique<Integer>(*integer);
	}
	else
	{
		item = std::make_unique<Id>(tag);
	}

}
Object::Object(Object&& other) :
	item(std::move(other.item))
{
}

Object& Object::operator=(Object&& other)
{
	item = std::move(other.item);
	return *this;
}

Object::operator bool() const
{
	return item.get();
}

bool Object::Trivial() const
{
	return operator==(boolean(true));
}


bool Object::operator==(const Object& other) const
{
	return *item == *other.item;
}

bool Object::Match(const Object& other, const Knowledge& knowledge) const
{
	return item->Match(*other.item, knowledge);
}

size_t Object::Hash() const
{
	return reinterpret_cast<size_t>(item.get());
}



}
}
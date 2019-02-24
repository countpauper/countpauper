#include "stdafx.h"
#include "Array.h"

namespace Angel
{
namespace Logic
{

Array::Array()
{
}

Array::Array(Object&& value)
{
	Append(std::move(value));
}

Array::Array(Array&& other) :
	std::vector<Object>(std::move(other))
{
}


bool Array::operator==(const Item& other) const
{
	if (auto array = dynamic_cast<const Array*>(&other))
	{
		for (const auto& e : *array)
		{
			if (std::find(begin(), end(), e) == end())
				return false;
		}
		for (const auto& e : *this)
		{
			if (std::find(array->begin(), array->end(), e) == array->end())
				return false;
		}
		return true;
	}
	return false;
}

bool Array::Match(const Item& item, const Knowledge& knowledge) const
{
	if (auto array = dynamic_cast<const Array*>(&item))
	{
		if (size() != array->size())
			return false;
		auto it = array->begin();
		for (const auto& e : *this)
		{
			if (!e.Match(*it, knowledge))
				return false;
			++it;	// TODO: zip
		}
		return true;
	}
	return false;
}


void Array::Append(Object&& value)
{
	if (auto array = value.Cast<Array>())
	{
		Merge(std::move(*array));
	}
	else if (value)
	{
		emplace_back(std::move(value));
	}
}

void Array::Merge(Array&& other)
{
	for (auto& e : other)
		emplace_back(std::move(e));
}


Object array()
{
	return Object(std::make_unique<Array>());
}

Object array(Array&& left, Array&& right)
{
	auto result = array();
	result.Cast<Array>()->Merge(std::move(left));
	result.Cast<Array>()->Merge(std::move(right));
	return result;
}

Object array(Array&& left, Object&& right)
{
	auto result = array();
	result.Cast<Array>()->Merge(std::move(left));
	result.Cast<Array>()->Append(std::move(right));
	return result;
}

Object array(Object&& left, Array&& right)
{
	auto result = array(std::move(left));
	result.Cast<Array>()->Merge(std::move(right));
	return result;
}



}
}
#include "stdafx.h"
#include "Array.h"

namespace Angel
{
namespace Logic
{

Array::Array()
{
}

Array::Array(Element&& value)
{
	emplace_back(std::move(value));
}

Array::Array(Array&& other) :
	std::vector<Element>(std::move(other))
{
}


bool Array::operator==(const Value& value) const
{
	if (auto array = dynamic_cast<const Array*>(&value))
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

bool Array::Match(const Value& value, const Knowledge& knowledge) const
{
	if (auto array = dynamic_cast<const Array*>(&value))
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


void Array::Append(Element&& value)
{
	emplace_back(std::move(value));
}

void Array::Merge(Array&& other)
{
	for (auto& e : other)
		emplace_back(std::move(e));
}


Element array()
{
	return Element(std::make_unique<Array>());
}

Element array(Array&& left, Array&& right)
{
	auto result = array();
	result.Cast<Array>()->Merge(std::move(left));
	result.Cast<Array>()->Merge(std::move(right));
	return result;
}

Element array(Array&& left, Element&& right)
{
	auto result = array();
	result.Cast<Array>()->Merge(std::move(left));
	result.Cast<Array>()->Append(std::move(right));
	return result;
}

Element array(Element&& left, Array&& right)
{
	auto result = array(std::move(left));
	result.Cast<Array>()->Merge(std::move(right));
	return result;
}



}
}
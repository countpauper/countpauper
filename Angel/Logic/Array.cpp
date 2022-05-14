#include "stdafx.h"
#include "Array.h"
#include "Boolean.h"
#include "Set.h"

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

bool Array::operator==(const Expression& other) const
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

bool Array::Match(const Expression& expr, const Knowledge& knowledge) const
{
	if (auto array = dynamic_cast<const Array*>(&expr))
	{
		if (size() != array->size())
			return false;
		auto it = array->begin();
		for (const auto& e : *this)
		{
			if (!e.Match(**it, knowledge))
				return false;
			++it;	// TODO: zip
		}
		return true;
	}
	return false;
}

Object Array::Cast(const std::type_info& t, const Knowledge& k) const
{
    if (t == typeid(Boolean))
    {
        return boolean(!empty());
    }
    else if (t == typeid(Set))
    {
        return Object(); // TODO: copy all members 
    }
    throw CastException<Array>(t);
}

void Array::Append(Object&& value)
{
	if (auto array = value.As<Array>())
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
    return Create<Array>();
}

Object array(Array&& left, Array&& right)
{
	auto result = array();
	result.As<Array>()->Merge(std::move(left));
	result.As<Array>()->Merge(std::move(right));
	return result;
}

Object array(Array&& left, Object&& right)
{
	auto result = array();
	result.As<Array>()->Merge(std::move(left));
	result.As<Array>()->Append(std::move(right));
	return result;
}

Object array(Object&& left, Array&& right)
{
	auto result = array(std::move(left));
	result.As<Array>()->Merge(std::move(right));
	return result;
}

}
}
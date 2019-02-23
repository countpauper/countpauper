#pragma once
#include <vector>
#include "Id.h"
#include "Object.h"
#include "Collection.h"

namespace Angel
{
namespace Logic
{

class Array : public Collection, public std::vector<Object>
{
public:
	Array();
	explicit Array(Object&& value);
	template<class ...Args>
	explicit Array(Object&& first, Args... args) :
		Array(std::move(first))
	{
		Merge(Array(std::forward<Args>(args)...));
	}

	Array(const Array&) = delete;
	Array& operator=(const Array&) = delete;
	Array(Array&& other);

	bool operator==(const Item& other) const override;
	bool Match(const Item& other, const Knowledge& knowledge) const override;
	void Append(Object&& value);
	void Merge(Array&& other);
};

Object array();
Object array(Array&& left, Array&& right);
Object array(Array&& left, Object&& right);
Object array(Object&& left, Array&& right);


template<class ...Args>
Object array(Args... args)
{
	return Object(std::make_unique<Array>(std::forward<Args>(args)...));
}

}
}

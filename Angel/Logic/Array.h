#pragma once
#include <vector>
#include "Id.h"
#include "Element.h"

namespace Angel
{
namespace Logic
{

class Array : public Value, public std::vector<Element>
{
public:
	Array();
	explicit Array(Element&& value);
	template<class ...Args>
	explicit Array(Element&& first, Args... args) :
		Array(std::move(first))
	{
		Merge(Array(std::forward<Args>(args)...));
	}

	Array(const Array&) = delete;
	Array& operator=(const Array&) = delete;
	Array(Array&& other);

	bool operator==(const Value& other) const override;
	bool Match(const Value& other, const Knowledge& knowledge) const override;
	void Append(Element&& value);
	void Merge(Array&& other);
};

Element array();
Element array(Array&& left, Array&& right);
Element array(Array&& left, Element&& right);
Element array(Element&& left, Array&& right);


template<class ...Args>
Element array(Args... args)
{
	return Element(std::make_unique<Array>(std::forward<Args>(args)...));
}

}
}

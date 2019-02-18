#pragma once
#include <unordered_set>
#include "Element.h"
#include "AllTrue.h"

namespace Angel
{
namespace Logic
{

class Array;

class Set : public Value, public std::unordered_set<Element>
{
public:
	Set();
	explicit Set(Array&& array);
	explicit Set(Element&& value);

	template<class ...Args, class = std::enable_if_t <
		all_true < std::is_convertible<Args, Element>{}... > {}
	>>
		explicit Set(Element&& first, Args... args) :
		Set(std::move(first))
	{
		Merge(Set(std::forward<Args>(args)...));
	}

	Set(const Set&) = delete;
	Set& operator=(const Set&) = delete;
	Set(Set&& other);
	bool operator==(const Value& other) const override;
	bool Match(const Value& other, const Knowledge& knowledge) const override;
	void Append(Element&& value);
	void Merge(Set&& other);
};

Element set();
Element set(Array&& array);

template<class ...Args, class = std::enable_if_t <
	all_true < std::is_convertible<Args, Element>{}... > {}
>>
Element set(Args... args)
{
	return Element(std::make_unique<Set>(std::forward<Args>(args)...));
}

}
}

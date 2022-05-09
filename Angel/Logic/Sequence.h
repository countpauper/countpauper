#pragma once
#include <vector>
#include "Collection.h"
#include "Object.h"
#include "AllTrue.h"

namespace Angel
{
namespace Logic
{

class Array;

// A sequence is a comma separates list of items that can probably be removed since it's the same as an Array
class Sequence : public Collection, public std::vector<Object>
{
public:
	Sequence();
	explicit Sequence(Array&& array);
	explicit Sequence(Object&& value);
	
	template<class ...Args, class = std::enable_if_t<
		all_true < std::is_convertible<Args, Object>{}... > {}
	>>
	explicit Sequence(Object&& first, Args... args) :
		Sequence(std::move(first))
	{
		Merge(Sequence(std::forward<Args>(args)...));
	}

	Sequence(const Sequence&) = delete;
	Sequence& operator=(const Sequence&) = delete;
	Sequence(Sequence&& other);
	bool operator==(const Item& other) const override;
	bool Match(const Item& other, const Knowledge& knowledge) const override;
	void Append(Object&& value);
	void Merge(Sequence&& other);
};

Object sequence();
Object sequence(Array&& array);

template<class ...Args, class = std::enable_if_t <
	all_true < std::is_convertible<Args, Object>{}... > {}
>>
Object sequence(Args... args)
{
	return Object(std::make_unique<Sequence>(std::forward<Args>(args)...));
}

}
}

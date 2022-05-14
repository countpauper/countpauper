#pragma once
#include <unordered_set>
#include "Collection.h"
#include "Object.h"
#include "AllTrue.h"

namespace Angel
{
namespace Logic
{

class Array;

class Set : public Collection, public std::unordered_set<Object>
{
public:
	Set();
	explicit Set(Array&& array);
	explicit Set(Object&& value);

	template<class ...Args, class = std::enable_if_t <
		all_true < std::is_convertible<Args, Object>{}... > {}
	>>
		explicit Set(Object&& first, Args... args) :
		Set(std::move(first))
	{
		Merge(Set(std::forward<Args>(args)...));
	}

	Set(const Set&) = delete;
	Set& operator=(const Set&) = delete;
	Set(Set&& other);
	bool operator==(const Expression& other) const override;
	bool Match(const Expression& other, const Knowledge& knowledge) const override;
	void Append(Object&& value);
	void Merge(Set&& other);
protected:
    Object Cast(const std::type_info& t, const Knowledge& k) const override;
};

Object set();
Object set(Array&& array);

template<class ...Args, class = std::enable_if_t <
	all_true < std::is_convertible<Args, Object>{}... > {}
>>
Object set(Args... args)
{
	return Create<Set>(std::forward<Args>(args)...);
}

}
}

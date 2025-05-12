#pragma once

#include "Logic/Element.h"
#include <map>

namespace Angel::Logic
{

class Object;

class Set : public std::map<Object, Object>
{
public:
	Set() = default;
	Set(std::initializer_list<Object> setItems);
	template<typename T> 
	Set(std::initializer_list<T> setItems)
	{
		for(auto&& item: setItems)
		{
			emplace(std::make_pair(std::move(item), Boolean(true)));
		}
	}
	const Object* Find(const Object& o) const;
	operator bool() const;
	std::size_t Hash() const;
	bool operator==(const Set& rhs) const;
};

std::ostream& operator<<(std::ostream& os, const Set& set);

}

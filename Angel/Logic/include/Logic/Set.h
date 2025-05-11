#pragma once

#include "Logic/Element.h"
#include "Logic/Node.h"
#include <map>

namespace Angel::Logic
{

class Node;
class Set : public std::map<Node, Node>
{
public:
	Set() = default;
	Set(std::initializer_list<Node> setItems);
	template<typename T> 
	Set(std::initializer_list<T> setItems)
	{
		for(auto&& item: setItems)
		{
			emplace(std::make_pair(std::move(item), Boolean(true)));
		}
	}
	const Node* Find(const Node& o) const;
	operator bool() const;
	std::size_t Hash() const;
	bool operator==(const Set& rhs) const;
};

std::ostream& operator<<(std::ostream& os, const Set& set);

}

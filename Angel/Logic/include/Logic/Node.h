
#pragma once

#include <variant>

namespace Angel::Logic
{
    // To break the circular dependency by Object and container Objects like List and Set
    // All objects in containers are wrapped in a Node struct that is forward declared. 
    class Node;
}

namespace std
{
	template <>
	struct hash<Angel::Logic::Node>
	{
		size_t operator()(const Angel::Logic::Node& n) const;
	};
}
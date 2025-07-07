#pragma once 
#include <variant>

namespace Angel::Logic
{

class Expression;
class Container;
}

namespace std
{
	template <>
	struct hash<Angel::Logic::Expression>
	{
		size_t operator()(const Angel::Logic::Expression& n) const;
	};
}


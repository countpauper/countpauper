#pragma once

#include <map>
#include "Position.h"

namespace Game
{
	class Direction
	{
	public:
		enum class Value
		{
			North = 1,
			East = 2,
			South = 4,
			West = 8,
		};
		Direction(Value direction);
		Position Vector() const;
	protected:
		Value value;
		static std::map<Value, Position> vector;
	};

}	// ::Game

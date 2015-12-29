#include "stdafx.h"
#include "Direction.h"

namespace Game
{
	Direction::Direction(Direction::Value value) :
		value(value)
	{
	}
	
	Position Direction::Vector() const
	{
		return vector.at(value);
	}
	std::map<Direction::Value, Position> Direction::vector =
	{
		{ Direction::Value::North, Position(0, 1) },
		{ Direction::Value::East, Position(1, 0) },
		{ Direction::Value::South, Position(0, -1) },
		{ Direction::Value::West, Position(-1, 0) },
	};
}	// ::Game

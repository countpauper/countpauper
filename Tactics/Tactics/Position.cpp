#include "stdafx.h"
#include "Position.h"

namespace Game
{

    Position::Position() : x(0), y(0) 
    {
    }
    Position::Position(int x, int y) : x(x), y(y) 
    {
    }

	bool operator==(const Position& a, const Position& b)
	{
		return a.x == b.x && a.y == b.y;
	}
	bool operator!=(const Position& a, const Position& b)
	{
		return !operator==(a, b);
	}

} // ::Game
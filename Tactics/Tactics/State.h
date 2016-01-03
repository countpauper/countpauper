#pragma once

#include "Position.h"

namespace Game
{
	class Actor;

	class State
	{
	public:
		State(const Actor& actor);
		bool possible;
		Position position;
		unsigned mp;
	};
} // ::Game
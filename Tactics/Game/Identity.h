#pragma once
\
#include "Target.h"

namespace Game
{
	class State;

	class Identity : public Target
	{	// id to identify actor states or without actor for unit tests
	public:
		Identity() = default;
		virtual ~Identity() = default;
		virtual void Apply(const State& result) = 0;
	};

}   // ::Game


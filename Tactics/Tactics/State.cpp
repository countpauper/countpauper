#include "stdafx.h"
#include "State.h"
#include "Actor.h"

namespace Game
{
	State::State(const Actor& actor) :
		possible(false),
		position(actor.GetPosition()),
		mp(actor.GetMovePoints())
	{
	}
} // ::Game
#pragma once

#include <memory>
#include <vector>
#include "Engine/Random.h"
#include "Position.h"
#include "Direction.h"
#include "Body.h"
#include "Stats.h"
#include "Score.h"
#include "Item.h"
#include "Skills.h"
#include "Target.h"

namespace Game
{
	class Identity
	{	// id to identify actor states or without actor for unit tests
	public:
		Identity() = default;
		virtual ~Identity() = default;
		virtual std::wstring Description() const=0;
	};

}   // ::Game


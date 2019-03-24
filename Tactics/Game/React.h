#pragma once

#include "Action.h"

namespace Game
{
	class Game;
	class Skill;

	class React :
		public TargetedAction
	{
	public:
		React(const Identity& actor, const TargetedAction& target, const Skill& skill);
		void Render(const State& state) const override;
		void Act(IGame& game) const override;
	};

}   // ::Game
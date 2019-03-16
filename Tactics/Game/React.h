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
		React(const Actor& actor, const TargetedAction& target, const Skill& skill);
		void Render(const State& state) const override;
		Result Act(const IGame& game) const override;
	};

}   // ::Game
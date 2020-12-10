#pragma once

#include "Action.h"

namespace Game
{
	class Game;
	class Skill;

	class React :
		public TargetedAction,
		public DirectedAction
	{
	public:
		React(const Identity& actor, const Skill& skill, const Action& target, const Physics::Direction& trajectory);
		void Render(const State& state) const override;
		void Act(IGame& game) const override;
	};

}   // ::Game
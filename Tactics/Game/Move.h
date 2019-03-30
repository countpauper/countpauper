#pragma once

#include "Direction.h"
#include "State.h"
#include "Action.h"

namespace Game
{
    class Game;

    class Move :
        public TargetedAction,
		public DirectedAction
    {
    public:
        Move(const Identity& actor, const Skill& skill, const Position& destination);
        void Render(const State& state) const override;
        void Act(IGame& game) const override;
        std::wstring Description() const override;
    protected:
        Destination destination;
    };

}   // ::Game
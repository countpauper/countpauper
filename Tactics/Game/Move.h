#pragma once

#include "Direction.h"
#include "State.h"
#include "Action.h"

namespace Game
{
    class Game;

    class Move :
        public TargetedAction
    {
    public:
        Move(const Identity& actor, const Position& destination, const Skill& skill);
        void Render(const State& state) const override;
        void Act(IGame& game) const override;
        std::wstring Description() const override;
    protected:
        Destination destination;
    };

}   // ::Game
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
        Move(const Actor& actor, const Position& destination, const Skill& skill);
        void Render(const State& state) const override;
        Result Act(const IGame& game) const override;
        std::wstring Description() const override;
    protected:
        Destination destination;
    };

}   // ::Game
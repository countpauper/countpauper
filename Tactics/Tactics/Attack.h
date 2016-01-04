#pragma once

#include "State.h"
#include "Action.h"

namespace Game
{
    class Game;

    class Attack :
        public TargetedAction
    {
    public:
        Attack(Actor& target);
        void Render(const State& state) const override;
        State Act(const State& state, const Game& game) override;
        void React() override;
    };

    class Slash : public Attack
    {
    public:
        Slash(Actor& target);
    };

}   // ::Game
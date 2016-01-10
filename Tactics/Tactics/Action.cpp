#include "stdafx.h"
#include "Action.h"
#include "Move.h"
#include "Attack.h"
#include "Game.h"

namespace Game
{
    Action::Action() :
        cost(0)
    {
    }

    Action::Properties::Properties(unsigned cost) :
        cost(cost)
    {
    }

    TargetedAction::TargetedAction(Actor& target) :
        target(target)
    {
    }

    std::map<unsigned, std::function<Action*(const State& state, const Game& game)>> Action::keymap = 
    {
        { VK_UP, [](const State&, const Game&){ return new South(); } },
        { VK_RIGHT, [](const State&, const Game&){ return new East(); } },
        { VK_DOWN, [](const State&, const Game&){ return new North(); } },
        { VK_LEFT, [](const State&, const Game&){ return new West(); } },
        { VK_SPACE, [](const State& state, const Game& game)
        {
            auto target = game.FindTarget(state, Slash::properties.range);
            if (!target)
                return (Slash*)nullptr;
            return new Slash(*target);
        } },
    };

} // ::Game
#include "stdafx.h"
#include "Action.h"
#include "Move.h"
#include "Attack.h"
#include "Game.h"
#include "Skills.h"
#include "State.h"
namespace Game
{
    Action::Action()
    {
    }

    TargetedAction::TargetedAction(const Actor& target) :
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
            auto& skill = game.skills.front();
            auto targets = game.FindTargetsInRange(state, skill);
            if (targets.empty())
                return (Action*)nullptr;
            return skill.Action(*targets.front());
        } },
    };

} // ::Game
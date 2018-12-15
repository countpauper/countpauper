#include "stdafx.h"
#include "Action.h"
#include "Move.h"
#include "Attack.h"
#include "Game.h"
#include "Skills.h"
#include "State.h"
namespace Game
{
    Action::Action(const Actor& actor) :
        actor(actor)
    {
    }

    TargetedAction::TargetedAction(const Actor& actor, const Actor& target) :
        Action(actor),
        target(target)
    {
    }

    std::map<unsigned, std::function<Action*(const State& state, const Game& game)>> Action::keymap = 
    {
        { VK_UP, [](const State&, const Game& game){ return new South(*game.ActiveActor()); } },
        { VK_RIGHT, [](const State&, const Game& game){ return new East(*game.ActiveActor()); } },
        { VK_DOWN, [](const State&, const Game&  game){ return new North(*game.ActiveActor()); } },
        { VK_LEFT, [](const State&, const Game& game){ return new West(*game.ActiveActor()); } },
        { VK_SPACE, [](const State& state, const Game& game)
        {
            auto& skill = game.skills.front();
            const auto& actor = *game.ActiveActor();
            auto targets = game.FindTargetsInRange(state, skill);
            if (targets.empty())
                return (Action*)nullptr;
            return skill.CreateAction(actor, *targets.front());
        } },
    };

} // ::Game
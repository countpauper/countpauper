#include "stdafx.h"
#include <sstream>
#include "State.h"
#include "Actor.h"
#include "Action.h"

namespace Game
{
    State::State(const Actor& actor) :
        position(actor.GetPosition()),
        direction(actor.GetDirection()),
        mp(actor.GetMovePoints()),
        health(actor.health),
        loyalty(actor.GetTeam())
    {
    }

    GameState::GameState(IGame& parent) :
        parent(parent)
    {
    }
    void GameState::Adjust(Actor& actor, const State& actorState)
    {
        state.insert(std::make_pair(&actor, actorState));
    }


    void GameState::Act(const Action& action)
    {
        if (auto targetAction = dynamic_cast<const TargetedAction*>(&action))
        {
            auto state = Get(targetAction->target);
            auto newState = targetAction->React(state);
            Adjust(targetAction->target, newState);
        }
    }

    State GameState::Get(const Actor& actor) const
    {
        const auto actorIt = state.find(const_cast<Actor*>(&actor));
        if (actorIt != state.end())
            return actorIt->second;
        return parent.Get(actor);
    }

    void GameState::Apply()
    {
        for (auto& actorState : state)
        {
            parent.Adjust(*actorState.first, actorState.second);
        }
        parent.Apply();
    }

    std::wstring GameState::Description() const
    {
        std::wstringstream ss;
        for (const auto& actorState : state)
        {
            ss << actorState.first->name << L": health " << actorState.second.health.Description() << L", "; // L" @ (" << actorState.second.position.Description() << L"), ";
        }
        return ss.str();
    }

    GameChance::GameChance(std::unique_ptr<GameState> state, double chance) :
        state(std::move(state)),
        chance(chance)
    {
    }
    GameChance::GameChance(GameChance&& other) :
        state(std::move(other.state)),
        chance(other.chance)
    {
    }
} // ::Game
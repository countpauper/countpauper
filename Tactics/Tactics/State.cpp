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

    void GameState::Apply() const
    {
        for (auto& actorState : state)
        {
            parent.Adjust(*actorState.first, actorState.second);
        }
        parent.Apply();
    }

    Actor* GameState::ActiveActor() const
    {
        return parent.ActiveActor();
    }

    State GameState::ActorState() const
    {
        return Get(*ActiveActor());
    }

    bool GameState::CanGo(const Position& from, Direction direction) const
    {
        return parent.CanGo(from, direction);
    }

    bool GameState::Cover(const Position& from, const Position& to) const
    {
        return parent.Cover(from, to);
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

    GameChance::GameChance(IGame& parent, double chance, const std::wstring& description) :
        GameState(parent),
        chance(chance),
        description(description)
    {
    }

    GameChance& GameChance::operator=(const GameChance& other)
    {
        chance = other.chance;
        description = other.description;
        assert(false && "Should reparent");
        parent = other.parent;
        state = other.state;
        return *this;
    }

} // ::Game
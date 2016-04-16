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
        body(actor.body),
        damage(actor.AttackDamage()),
        mitigation(actor.Mitigation()),
        stats(actor.Statistics()),
        loyalty(actor.GetTeam())
    {
    }

    GameState::GameState(IGame& parent) :
        parent(parent)
    {
    }
    void GameState::Adjust(Actor& actor, const State& actorState)
    {
        auto insert = state.insert(std::make_pair(&actor, actorState));
        if (!insert.second)
            state.at(&actor) = actorState;
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

    bool GameState::CanBe(const Position& position) const
    {
        return parent.CanBe(position);
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
            ss << actorState.first->name << L": " << actorState.second.body.Description(actorState.second.stats.constitution) << L", "; // L" @ (" << actorState.second.position.Description() << L"), ";
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
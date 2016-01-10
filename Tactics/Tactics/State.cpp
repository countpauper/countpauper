#include "stdafx.h"
#include "State.h"
#include "Actor.h"

namespace Game
{
    State::State(const Actor& actor) :
        possible(true),
        position(actor.GetPosition()),
        direction(actor.GetDirection()),
        mp(actor.GetMovePoints()),
        hp(actor.GetHitPoints()),
        loyalty(actor.GetTeam())
    {
    }

    GameState::GameState() :
        parent(nullptr)
    {
    }

    GameState::GameState(const GameState& parent) :
        parent(&parent)
    {
    }
    void GameState::Adjust(const Actor& actor, const State& actorState)
    {
        state.insert(std::make_pair(&actor, actorState));
    }

    void GameState::Apply(Game& game) const
    {
        RecursiveApply(game, std::set<const Actor*>());
    }

    State GameState::Get(const Actor& actor) const
    {
        const auto actorIt = state.find(&actor);
        if (actorIt != state.end())
            return actorIt->second;
        else if (parent)
            return parent->Get(actor);
        else return State(actor);
    }

    void GameState::RecursiveApply(Game& game, std::set<const Actor*>& done) const
    {
        for (const auto& actorState : state)
        {
            if (done.count(actorState.first))
                continue;
            auto actor = const_cast<Actor*>(actorState.first);
            actor->Apply(actorState.second);
            done.insert(actorState.first);
        }
        if (parent)
            parent->RecursiveApply(game, done);
    }
} // ::Game
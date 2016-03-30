#pragma once

#include <map>
#include <set>
#include <memory>
#include <vector>
#include "Position.h"
#include "Direction.h"
#include "Health.h"
#include "Stats.h"
#include "IGame.h"

namespace Game
{
    class Actor;
    class Game;
    class Action;

    class State
    {
    public:
        State(const Actor& actor);
        Position position;
        Direction direction;
        unsigned mp;
        Body health;
        Damage mitigation;
        Stats stats;
        unsigned loyalty;
    };

    class GameState : public IGame
    {
    public:
        GameState();
        GameState(IGame& parent);
        void Adjust(Actor& actor, const State& state) override;
        void Apply() const override;
        State Get(const Actor& actor) const override;
        void Act(const Action& action);

        Actor* ActiveActor() const override;
        bool CanBe(const Position& position) const override;
        bool CanGo(const Position& from, Direction direction) const override;
        bool Cover(const Position& from, const Position& to) const override;
        std::wstring Description() const override;

        State ActorState() const;
    protected:
        void RecursiveApply(Game& game, std::set<const Actor*>& done) const;
        IGame& parent;
        std::map<Actor*, State> state;
    };

    class GameChance : public GameState
    {
    public:
        GameChance(IGame& parent, double chance, const std::wstring& description);
        GameChance& operator=(const GameChance& other);
        double chance;
        std::wstring description;
    };

    using GameChances= std::vector < GameChance > ;
} // ::Game
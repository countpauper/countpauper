#pragma once

#include <map>
#include <set>
#include <memory>
#include <vector>
#include "Position.h"
#include "Direction.h"
#include "Health.h"
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
        Health health;
        unsigned loyalty;
    };

    class Outcome
    {
    public:
        State state;
        double chance;
        std::wstring description;
    };

    using Outcomes = std::vector <Outcome> ;

    class GameState : public IGame
    {
    public:
        GameState();
        GameState(IGame& parent);
        void Adjust(Actor& actor, const State& state) override;
        void Apply() override;
        State Get(const Actor& actor) const override;
        void Act(const Action& action);
        std::wstring Description() const override;
    private:
        void RecursiveApply(Game& game, std::set<const Actor*>& done) const;
        IGame& parent;
        std::map<Actor*, State> state;
    };

    struct GameChance 
    {
        GameChance(std::unique_ptr<GameState> state, double chance, const std::wstring& description);
        GameChance(GameChance&& other);
        std::unique_ptr<GameState> state;
        double chance;
        std::wstring description;
    };

} // ::Game
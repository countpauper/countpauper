#pragma once

#include <map>
#include <set>
#include <memory>

#include "Position.h"
#include "Direction.h"
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
        bool possible;
        Position position;
        Direction direction;
        unsigned mp;
        unsigned hp;
        unsigned loyalty;
    };

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
        GameChance(std::unique_ptr<GameState> state, float chance);
        GameChance(GameChance&& other);
        std::unique_ptr<GameState> state;
        float chance;
    };

} // ::Game
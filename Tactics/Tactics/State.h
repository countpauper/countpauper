#pragma once

#include <map>
#include <set>
#include "Position.h"
#include "Direction.h"
namespace Game
{
    class Actor;
    class Game;

    class State
    {
    public:
        State(const Actor& actor);
        bool possible;
        Position position;
        Direction direction;
        unsigned mp;
        unsigned hp;
    };

    class GameState
    {
    public:
        GameState();
        GameState(const GameState& parent);
        void Adjust(const Actor& actor, const State& state);
        void Apply(Game& game) const; 
        State Get(const Actor& actor) const;
    private:
        void RecursiveApply(Game& game, std::set<const Actor*>& done) const;
        const GameState* parent;
        std::map<const Actor*, State> state;
    };
} // ::Game
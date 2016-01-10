#pragma once

#include <map>
#include <functional>
#include "Direction.h"
#include "State.h"

namespace Game
{
    class Actor;
    class Game;

    class Action
    {
    public:
        Action();
        virtual ~Action() = default;
        virtual State Act(const State& state, const Game& game)=0;
        virtual void Render(const State& state) const = 0;
        static std::map<unsigned, std::function<Action*(const State& state, const Game& game)>> keymap;
        struct Properties
        {
            Properties(unsigned cost);
            unsigned cost;
        };
        virtual Properties& GetProperties() const = 0;
    protected:
        unsigned cost;
    };
    
    class TargetedAction : public Action
    {
    public:
        TargetedAction(Actor& target);
        virtual void React() = 0;
    protected:
        Actor& target;
    };
}   // ::Game


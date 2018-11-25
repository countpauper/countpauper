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
        virtual GameChances Act(IGame& game) = 0;
        virtual void Render(const State& state) const = 0;
        virtual std::wstring Description() const = 0;
        static std::map<unsigned, std::function<Action*(const State& state, const Game& game)>> keymap;
        static std::map<std::wstring, std::function<Action*(const State& state, const Game& game)>> typemap;
    };
    
    class TargetedAction : public Action
    {
    public:
        TargetedAction(Actor& target);
        Actor& target;
    };
}   // ::Game


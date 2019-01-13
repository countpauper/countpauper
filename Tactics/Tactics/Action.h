#pragma once

#include <map>
#include <functional>
#include <memory>
#include "Game/Trajectory.h"

namespace Game
{
    class Actor;
    class Game;
    class Skill;
    class IGame;
    class GameState;
    class State;


    class Action
    {
    public:
        Action(const Skill& skill, const Actor& actor, Trajectory trajectory);
        virtual ~Action() = default;
        class Result
        {
        public:
            Result();
            Result(const IGame& parent, const Actor& actor);
            Result(const IGame& parent, const Actor& actor, const Skill& skill);
            
            Result(const Result&) = delete;
            Result& operator=(const Result&) = delete;
            Result(Result&& other);
            Result& operator=(Result&& other);
            operator bool() const;

            std::unique_ptr<GameState> state;
            double chance;
            std::wstring description;
        };
        virtual Result Act(const IGame& game) const = 0;
        virtual Result Fail(const IGame& game) const = 0;
        virtual void Render(const State& state) const = 0;
        virtual std::wstring Description() const = 0;
        static std::map<unsigned, std::function<Action*(const State& state, const Game& game)>> keymap;
        static std::map<std::wstring, std::function<Action*(const State& state, const Game& game)>> typemap;
        const Actor& actor;
        const Skill& skill;
        const Trajectory trajectory;
    };
    
    class TargetedAction : public Action
    {
    public:
        TargetedAction(const Skill& skill, const Actor& actor, const Actor& target, Trajectory trajectory);
        const Actor& target;
    };

}   // ::Game


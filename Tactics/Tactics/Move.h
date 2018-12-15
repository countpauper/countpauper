#pragma once

#include "Direction.h"
#include "State.h"
#include "Action.h"

namespace Game
{
    class Game;

    class Move :
        public Action
    {
    public:
        Move(const Actor& actor, Direction direction);
        void Render(const State& state) const override;
        std::unique_ptr<GameState> Act(const IGame& game) const override;
        std::wstring Description() const override;
    protected:
        Direction direction;
    };

    class North : public Move
    {
    public:
        North(const Actor& actor);
    };
    class East : public Move
    {
    public:
        East(const Actor& actor);
    };
    class South : public Move
    {
    public:
        South(const Actor& actor);
    };
    class West : public Move
    {
    public:
        West(const Actor& actor);
    };

}   // ::Game
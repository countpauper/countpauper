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
        Move(Direction direction);
        void Render(const State& state) const override;
        GameChances Act(IGame& game) override;
        std::wstring Description() const override;
    protected:
        Direction direction;
    };

    class North : public Move
    {
    public:
        North();
    };
    class East : public Move
    {
    public:
        East();
    };
    class South : public Move
    {
    public:
        South();
    };
    class West : public Move
    {
    public:
        West();
    };

}   // ::Game
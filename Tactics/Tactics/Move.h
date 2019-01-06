#pragma once

#include "Game/Direction.h"
#include "State.h"
#include "Action.h"

namespace Game
{
    class Game;

    class Move :
        public Action
    {
    public:
        Move(const Actor& actor, const Position& destination, const Skill& skill, Trajectory trajectory);
        void Render(const State& state) const override;
        Result Act(const IGame& game) const override;
        Result Fail(const IGame& game) const override;
        std::wstring Description() const override;
    protected:
        Position destination;
    };

}   // ::Game
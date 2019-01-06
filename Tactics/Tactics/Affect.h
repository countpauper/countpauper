#pragma once

#include "State.h"
#include "Action.h"

namespace Game
{
class Game;
class Skill;

class Affect :
    public TargetedAction
{
public:
    Affect(const Actor& actor, const Actor& target, const Skill& skill, Trajectory trajectory);
    void Render(const State& state) const override;
    Result Act(const IGame& game) const override;
    Result Fail(const IGame& game) const override;
    std::wstring Description() const override;
};

}   // ::Game
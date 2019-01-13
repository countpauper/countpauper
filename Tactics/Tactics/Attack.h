#pragma once

#include "Action.h"

namespace Game
{
class Game;
class Skill; 

class Attack :
    public TargetedAction
{
public:
    Attack(const Actor& actor, const Actor& target, const Skill& skill, Trajectory trajectory);
    void Render(const State& state) const override;
    Result Act(const IGame& game) const override;
    Result Fail(const IGame& game) const override;
    std::wstring Description() const override;
};

}   // ::Game
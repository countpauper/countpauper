#pragma once

#include "State.h"
#include "Action.h"

namespace Game
{
class Game;
class Skill; 
class Attack :
    public TargetedAction
{
public:
    Attack(Actor& target, const Skill& skill);
    void Render(const State& state) const override;
    GameChances Act(IGame& game) override;
    std::wstring Description() const override;
protected:
    const Skill& skill;
};

}   // ::Game
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
    Attack(const Actor& target, const Skill& skill);
    void Render(const State& state) const override;
    std::unique_ptr<GameState> Act(const IGame& game) const override;
    std::wstring Description() const override;
protected:
    const Skill& skill;
};

}   // ::Game
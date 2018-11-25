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
    // TODO: which object should implement this? not skill, not state really ...
    static std::vector<const Skill*> Combo(const Actor& actor, const Skill& previous);
    static const Skill* PickCombo(const std::vector<const Skill*>& combos, const State& actor, const State& target);
    static void Attack::Strike(const Skill& strike, State& attacker, State& target);

    const Skill& skill;
};

}   // ::Game
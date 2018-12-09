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
    std::unique_ptr<GameState> Act(IGame& game) override;
    std::wstring Description() const override;
protected:
    // TODO: which object should implement this? not skill, not state really ...
    static std::vector<const Skill*> Combo(const Actor& actor, const Skill& previous);
    static std::vector<const Skill*> React(const Actor& actor, const Skill& previous);
    static const Skill* Pick(const std::vector<const Skill*>& combos, const State& actor, const State& target);

    static void Strike(const Skill& strike, State& attacker, State& target);
    static std::pair<const Skill*, float> PicReaction(const Skill& attack, std::vector<const Skill*&> reactions, const State& target);

    const Skill& skill;
};

}   // ::Game
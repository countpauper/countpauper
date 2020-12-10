#pragma once

#include "Action.h"

namespace Game
{
class Game;
class Skill; 

class Attack :
    public TargetedAction,
	public AimedAction,
	public DirectedAction
{
public:
    Attack(const Identity& actor, const Skill& skill, const Identity& target, Physics::Direction trajectory, const Part* part);
    void Render(const State& state) const override;
    void Act(IGame& game) const override;
};

}   // ::Game
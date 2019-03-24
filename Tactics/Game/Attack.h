#pragma once

#include "Action.h"

namespace Game
{
class Game;
class Skill; 

class Attack :
    public DirectedAction
{
public:
    Attack(const Identity& actor, const Identity& target, const Skill& skill, Direction trajectory, const Part& part);
    void Render(const State& state) const override;
    void Act(IGame& game) const override;
};

}   // ::Game
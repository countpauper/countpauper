#pragma once

#include "Action.h"

namespace Game
{
class Game;
class Skill; 

class Attack :
    public AimedAction
{
public:
    Attack(const Actor& actor, const Actor& target, const Skill& skill, const Body::Part& part);
    void Render(const State& state) const override;
    void Act(IGame& game) const override;
};

}   // ::Game
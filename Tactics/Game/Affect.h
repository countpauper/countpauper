#pragma once

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
};

}   // ::Game
#pragma once

#include "Action.h"

namespace Game
{
class Game;
class Skill;

class Affect :
	public TargetedAction,
    public AimedAction
{
public:
    Affect(const Identity& actor, const Skill& skill, const Identity& target, const Part& part);
};

}   // ::Game
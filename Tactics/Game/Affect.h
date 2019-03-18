#pragma once

#include "Action.h"

namespace Game
{
class Game;
class Skill;

class Affect :
    public AimedAction
{
public:
    Affect(const Identity& actor, const Identity& target, const Skill& skill, const Part& part);
};

}   // ::Game
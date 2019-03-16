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
    Affect(const Actor& actor, const Actor& target, const Skill& skill, const Body::Part& part);
};

}   // ::Game
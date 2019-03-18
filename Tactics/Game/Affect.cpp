#include "stdafx.h"
#include <gl/GL.h>
#include "Affect.h"
#include "State.h"
#include "Skills.h"

namespace Game
{

Affect::Affect(const Identity& actor, const Identity& target, const Skill& skill, const Part& part) :
    AimedAction(skill, actor, target, part)
{
}


} // ::Game
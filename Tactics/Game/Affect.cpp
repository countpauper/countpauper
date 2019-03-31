#include "stdafx.h"
#include <gl/GL.h>
#include "Affect.h"
#include "State.h"
#include "Skills.h"

namespace Game
{

Affect::Affect(const Identity& actor, const Skill& skill, const Identity& target, const Part* part) :
	TargetedAction(actor, skill, target),
    AimedAction(part)
{
}


} // ::Game
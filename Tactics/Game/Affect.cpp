#include "stdafx.h"
#include <gl/GL.h>
#include "Affect.h"
#include "State.h"
#include "Actor.h"
#include "Skills.h"

namespace Game
{

Affect::Affect(const Actor& actor, const Actor& target, const Skill& skill, Trajectory trajectory) :
    TargetedAction(skill, actor, target, trajectory)
{
}


} // ::Game
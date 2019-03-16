#include "stdafx.h"
#include <gl/GL.h>
#include "React.h"
#include "State.h"
#include "Actor.h"
#include "Skills.h"

namespace Game
{

React::React(const Actor& actor, const TargetedAction& target, const Skill& skill) :
	TargetedAction(skill, actor, target)
{
}

Action::Result React::Act(const IGame& game) const
{
	State defender = game.Get(actor);
	const auto& action = static_cast<const TargetedAction&>(target);
	State attacker(game.Get(action.actor));
	Result ret(game, actor);
	auto chance = defender.Chance(skill, attacker);
	ret.chance = double(chance.Value()) / 100.0;

	defender.Spent(skill.mp);
	defender.Engage(skill);
	ret.state->Adjust(actor, defender);
	// TODO: only supports failing other now, additional effects? 
	// TODO: doesn't explicitly fail, since act creates a new state. caller should do that if can't be impossible 
	//		or better, attacker should already have engaged in the state before the reaction   action.Fail(game, Description());
	attacker.Spent(action.skill.mp);
	attacker.Engage(action.skill);
	ret.state->Adjust(action.actor, attacker);
	ret.description = actor.Description() + L" " + skill.name + L"(" + chance.Description() + L"%) " + target.Description();
	return ret;
}

void React::Render(const State& state) const
{
	glColor4ub(0, 255, 0, 255);
	glPushMatrix();
	glTranslatef(float(state.position.x) + 0.5f, 0.5f, float(state.position.y) + 0.5f);
	Position v = target.GetPosition() - state.position;
	glBegin(GL_LINES);
	glVertex3f(0, 0, 0);
	glVertex3f(float(v.x), 0, float(v.y));
	glEnd();

	glRasterPos3i(0, 0, 1); // set start position
	glListBase(1000);
	std::string text(skill.name.begin(), skill.name.end());
	glCallLists(text.size(), GL_UNSIGNED_BYTE, text.c_str());

	glPopMatrix();
}



} // ::Game
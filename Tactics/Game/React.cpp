#include "stdafx.h"
#include <gl/GL.h>
#include "React.h"
#include "IGame.h"
#include "State.h"
#include "Skills.h"

namespace Game
{

React::React(const Identity& actor, const Skill& skill, const Action& target, const Physics::Direction& trajectory) :
	TargetedAction(actor, skill, target),
	DirectedAction(trajectory)
{
}

void React::Act(IGame& game) const
{
	auto chance = Chance(game);
	State defender = game.Get(actor);
	Engage(defender);
	game.Adjust(actor, defender, skill.name);

	const auto& action = dynamic_cast<const Action&>(target);
	State attacker(game.Get(action.actor));
	action.Engage(attacker);
	game.Adjust(action.actor, attacker, actor.Description() + L" " + skill.name + L"(" + chance.Description() + L"%) " + target.Description());
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
	glCallLists(static_cast<GLsizei>(std::min(65535ULL,text.size())), GL_UNSIGNED_BYTE, text.c_str());

	glPopMatrix();
}



} // ::Game
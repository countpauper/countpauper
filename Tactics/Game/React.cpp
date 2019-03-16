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

void React::Act(IGame& game) const
{
	auto chance = Chance(game);
	State defender = game.Get(actor);
	const auto& action = dynamic_cast<const TargetedAction&>(target);
	State attacker(game.Get(action.actor));

	defender.Spent(skill.mp);
	defender.Engage(skill);
	game.Adjust(actor, defender, skill.name);

	attacker.Spent(action.skill.mp);
	attacker.Engage(action.skill);
	attacker.direction = Direction(defender.position - attacker.position);

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
	glCallLists(text.size(), GL_UNSIGNED_BYTE, text.c_str());

	glPopMatrix();
}



} // ::Game
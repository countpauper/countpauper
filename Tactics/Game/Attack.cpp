#include "stdafx.h"
#include <gl/GL.h>
#include "Engine/Text.h"
#include "Engine/Image.h"
#include "IGame.h"
#include "Skills.h"
#include "Attack.h"
#include "State.h"

namespace Game
{

Attack::Attack(const Identity& actor, const Skill& skill, const Identity& target, Direction trajectory, const Part* part) :
	TargetedAction(actor, skill, target),
	AimedAction(part),
    DirectedAction(trajectory)
{
}

void Attack::Act(IGame& game) const
{
	State attacker = game.Get(actor);
	auto& targetActor = dynamic_cast<const Identity&>(target);
	State victim(game.Get(targetActor));
	if (!part)
	{
		Engage(attacker);
		game.Adjust(actor, attacker, skill.name);
		game.Adjust(targetActor, victim, L"Miss " + skill.name);
		return;
	}
	auto skillLevel = attacker.SkillLevel(skill, &victim);
	if (skillLevel.Value() == 0)
	{
		Engage(attacker);
		game.Adjust(actor, attacker, skill.name);
		game.Adjust(targetActor, victim, L"Resist " + skill.name + L":" + skillLevel.Description());
		return;
	}
	auto damage = attacker.AttackDamage(skill, skillLevel);
	Damage pain = damage - victim.Mitigation(*part);
	if (!victim.Hurt(*part, pain, actor.Description()+L"'"+skill.description))	
	{
		Engage(attacker);
		game.Adjust(actor, attacker, skill.name);
		game.Adjust(targetActor, victim, L"Mitigate " + skill.name + L"@" + part->Name() + L":" + pain.ActionDescription());
		return;
	}
	Engage(attacker);
	game.Adjust(actor, attacker, skill.name);
	game.Adjust(targetActor, victim, L"Hit " + skill.name + L"@" + part->Name() + L":" + pain.ActionDescription());
}

void Attack::Render(const State& state) const
{
    glColor4ub(255, 0, 0, 255);
    glPushMatrix();
    glTranslatef(float(state.position.x) + 0.5f, 0.5f, float(state.position.y) + 0.5f);
    Position v = target.GetPosition() - state.position;
    glBegin(GL_LINES);
    glVertex3f(0, 0, 0);
    glVertex3f(float(v.x), 0, float(v.y));
    glEnd();

    glColor4ub(255, 255, 255, 255);

    // TODO: billboard by setting the projection matrix 3x3 to identity
    Engine::Image::Bind bind(skill.Icon());
    glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 1.0f);
        glVertex3f(0.25f, -1.0f, 0.25f);

        glTexCoord2f(1.0f, 1.0f);
        glVertex3f(0.75, 1.0f, 0.25f);

        glTexCoord2f(1.0f, 0.0f);
        glVertex3f(0.75f, 0.75f, 1.0f);

        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(0.25f, 0.75f, 1.0f);
    glEnd();

    glPopMatrix();
}



} // ::Game
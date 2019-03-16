#include "stdafx.h"
#include <gl/GL.h>
#include "Engine/Text.h"
#include "Engine/Random.h"
#include "Engine/Image.h"
#include "Skills.h"
#include "Attack.h"
#include "State.h"
#include "Actor.h"

namespace Game
{

Attack::Attack(const Actor& actor, const Actor& target, const Skill& skill, const Body::Part& part) :
    AimedAction(skill, actor, target, part)
{
}

Action::Result Attack::Act(const IGame& game) const
{
    State attacker = game.Get(actor);
    State victim(game.Get(static_cast<const Actor&>(target)));

	attacker.direction = Direction(victim.position - attacker.position);
	auto skillLevel = attacker.SkillLevel(skill, &victim);
    if (skillLevel.Value() == 0)
        return Fail(game, std::wstring(L"resist(")+skillLevel.Description()+L")");
    // TODO: Fail due to miss? ret.chance = double(attacker.Chance(skill, victim).Value()) / 100.0;
    auto damage = attacker.AttackDamage(skill, skillLevel);

    if (!victim.Hurt(part, damage, actor.Description()))
        return Fail(game, L"mitigated");
    attacker.Spent(skill.mp);
    attacker.Engage(skill);

    Result ret(game, actor);
    ret.state->Adjust(actor, attacker);
    ret.state->Adjust(static_cast<const Actor&>(target), victim);
    ret.description = actor.Description() + L" "+skill.name;
    ret.description += L" " + target.Description() + L":" + damage.ActionDescription();
    return std::move(ret);
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
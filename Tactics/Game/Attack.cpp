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

Attack::Attack(const Actor& actor, const Actor& target, const Skill& skill, Trajectory trajectory) :
    TargetedAction(skill, actor, target, trajectory)
{
}

std::wstring Attack::Description() const
{
    return skill.name;
}

Anatomy HitLocation(const State& attacker, const Skill& skill, const State& victim, Trajectory trajectory)
{
    assert(!attacker.direction.Prone());  // prone not supported yet here
    if (victim.direction.Prone())
    {
        return Anatomy();
    }
    auto targeting = skill.target;
    if (targeting.count(Targeting::Swing))
    {
        Anatomy origin = attacker.Origin(skill, trajectory);
        // TODO: mirror for left arm/tail
        Anatomy facing(attacker.direction, victim.direction, 0);    // TODO: sensing body part or height difference
        return Anatomy(origin, facing);
    }
    else if (targeting.count(Targeting::Center))
    {
        auto height = victim.body.Length();
        double middle = static_cast<double>(height) / 2.0;
        auto hitHeight = static_cast<int>(std::round(middle + Engine::Random().Normal(1.0)));
        if (hitHeight < 0 || hitHeight >= static_cast<int>(height))
            return Anatomy();
        return Anatomy(attacker.direction, victim.direction, hitHeight);
    }
    assert(false);  // unsupported for now
    return Anatomy();
}

Action::Result Attack::Act(const IGame& game) const
{
    State attacker = game.Get(actor);
    State victim(game.Get(target));
    attacker.direction = Direction(victim.position - attacker.position);

    if (!attacker.IsPossible(skill, victim))
        return Result();

    auto location = HitLocation(attacker, skill, victim, trajectory);
    if (!location)
        return Fail(game, L"miss");
    auto skillLevel = attacker.SkillLevel(skill, &victim);
    if (skillLevel.Value() == 0)
        return Fail(game, skillLevel.Description());
    // TODO: Fail due to miss? ret.chance = double(attacker.Chance(skill, victim).Value()) / 100.0;
    auto damage = attacker.AttackDamage(skill, skillLevel);
    if (!victim.Hurt(location, damage, actor.Description()))
        return Fail(game, L"missed");
    attacker.mp -= skill.mp;
    attacker.Engage(skill);

    Result ret(game, actor);
    ret.state->Adjust(actor, attacker);
    ret.state->Adjust(target, victim);
    ret.description = actor.Description() + L" "+skill.name;
    ret.description += L" " + target.Description() + L":" + damage.ActionDescription();
    return std::move(ret);
}

Action::Result Attack::Fail(const IGame& game, const std::wstring& reason) const
{
    State attacker = game.Get(actor);
    State victim(game.Get(target));
    attacker.direction = Direction(victim.position - attacker.position);
    if (!attacker.IsPossible(skill, victim))
        return Result();
    Result ret(game, actor);
    attacker.mp -= skill.mp;
    attacker.Engage(skill);
    ret.state->Adjust(actor, attacker);
    ret.description = actor.Description() + L" " + skill.name + L" "+reason +L" " + target.Description();
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
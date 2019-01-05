#include "stdafx.h"
#include <gl/GL.h>
#include "Engine/Text.h"
#include "Attack.h"
#include "Game.h"
#include "Actor.h"
#include "Skills.h"

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

std::unique_ptr<GameState> Attack::Act(const IGame& game) const
{
    State attacker = game.Get(actor);
    State victim(game.Get(target));
    attacker.direction = Direction(victim.position - attacker.position);
   
    if (!attacker.IsPossible(skill, victim))
        return nullptr;
    AttackVector attackVector(trajectory, 0);
    AttackVector faceVector(attacker.direction, victim.direction, 0);
    AttackVector hitLocation(attackVector, faceVector);

    auto ret = std::make_unique<GameState>(game, actor);
    auto damage = attacker.AttackDamage(skill) - victim.Mitigation();
    attacker.mp -= skill.mp;
    victim.body.Hurt(hitLocation, damage.Wound(actor.Description()));

    ret->Adjust(actor, attacker);
    ret->Adjust(target, victim);
    return std::move(ret);
}

std::unique_ptr<GameState> Attack::Fail(const IGame& game) const
{
    State attacker = game.Get(actor);
    State victim(game.Get(target));
    attacker.direction = Direction(victim.position - attacker.position);
    if (!attacker.IsPossible(skill, victim))
        return nullptr;
    auto ret = std::make_unique<GameState>(game, actor);
    attacker.mp -= skill.mp;
    ret->Adjust(actor, attacker);
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
#include "stdafx.h"
#include <gl/GL.h>
#include "Attack.h"
#include "Game.h"
#include "Actor.h"
#include "Skills.h"

namespace Game
{

Attack::Attack(Actor& target, const Skill& skill) :
    TargetedAction(target),
    skill(skill)
{
}

std::wstring Attack::Description() const
{
    return skill.name;
}

GameChances Attack::Act(IGame& game)
{
    auto& actor = *game.ActiveActor();
    State attacker = game.Get(actor);
    State victim(game.Get(target));
    GameChances ret;

    std::vector<const Skill*> combos = { &skill };
    while (!combos.empty())
    {
        bool anyPossible = false;
        auto combo = PickCombo(combos, attacker, target);
        if (combo)
        {
            if (ret.empty())
            {
                ret.emplace_back(GameChance(game, 1.0, L""));
            }
            attacker.mp -= combo->mp;
            auto damage = attacker.AttackDamage() - victim.Mitigation();
            target.body.Hurt(AttackVector({ Plane::All, 0 }), damage.Wound(actor.name));

            ret.back().Adjust(actor, attacker);
            ret.back().Adjust(target, victim);
            ret.back().description += combo->name + L" : " + damage.ActionDescription() + L", ";
            combos = Combo(actor, *combo);
        }
        else
        {
            combos.clear();
        }
    }

    return ret;
}

const Skill* Attack::PickCombo(const std::vector<const Skill*>& combos, const State& actor, const State& target)
{
    for (auto combo : combos)
    {
        if (actor.IsPossible(*combo, target))
        {
            return combo;
        }
    }
    return nullptr;
}


std::vector<const Skill*> Attack::Combo(const Actor& actor, const Skill& previous)
{
    std::vector<const Skill*> possible;
    for (auto availableSkill : actor.GetSkills())
    {
        const auto skill = availableSkill.skill;
        if ((skill->trigger == Skill::Trigger::Combo) && (skill->Follows(previous)))
        {
            possible.push_back(skill);
        }
    }
    return possible;
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
    glPopMatrix();
}


} // ::Game
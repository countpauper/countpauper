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

std::unique_ptr<GameState> Attack::Act(IGame& game)
{
    auto& actor = *game.ActiveActor();
    State attacker = game.Get(actor);
    State victim(game.Get(target));

    std::unique_ptr<GameState> ret;

    std::vector<const Skill*> combos = { &skill };
    while (!combos.empty())
    {
        bool anyPossible = false;
        auto combo = Pick(combos, attacker, victim);
        if (combo)
        {
            /*
            auto reactions = React(target, *combo);
            auto reaction = Pick(reactions, victim, attacker);
            double remainingChance = 1;
            if (reaction)
            {
                auto chance = victim.DefendChance().Value()/100.0f;
                remainingChance -= chance;
                ret.emplace_back(GameChance(game, chance, reaction->name));
            }
            */
            if (!ret)
                ret = std::make_unique<GameState>(game);

            auto damage = attacker.AttackDamage() - victim.Mitigation();
            //ret.emplace_back(GameChance(game, remainingChance, combo->name + L" : " + damage.ActionDescription() + L", "));
            attacker.mp -= combo->mp;
            victim.body.Hurt(AttackVector({ Plane::All, 0 }), damage.Wound(actor.name));

            ret->Adjust(actor, attacker);
            ret->Adjust(target, victim);
            combos = Combo(actor, *combo);
        }
        else
        {
            combos.clear();
        }
    }

    return std::move(ret);
}

const Skill* Attack::Pick(const std::vector<const Skill*>& options, const State& actor, const State& target)
{
    for (auto option: options)
    {
        if (actor.IsPossible(*option, target))
        {
            return option;
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

std::vector<const Skill*> Attack::React(const Actor& actor, const Skill& previous)
{
    std::vector<const Skill*> possible;
    for (auto availableSkill : actor.GetSkills())
    {
        const auto skill = availableSkill.skill;
        if ((skill->trigger == Skill::Trigger::Defend) && (skill->Follows(previous)))
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
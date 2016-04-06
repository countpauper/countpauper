#include "stdafx.h"
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
        cost = skill.cost;
        range = skill.range;
    }

    std::wstring Attack::Description() const
    {
        return skill.name;
    }

    GameChances Attack::Act(IGame& game)
    {
        auto& actor = *game.ActiveActor();
        State state = game.Get(actor);

        if ((state.mp <= cost) ||
            (state.position.Distance(target.GetPosition()) > range) ||
            (game.Cover(state.position, target.GetPosition())))
        {
            return GameChances();
        }
        state.mp -= cost;

        int hitBonus = 5 + actor.GetSkillScore(skill) - target.Agility();
        double hitChance = std::min(1.0,std::max(0.0, double(hitBonus)*0.1));
        GameChances ret;
        ret.emplace_back(GameChance(game, hitChance, L"Hit"));
        ret.back().Adjust(actor, state);

        State targetResult(game.Get(target));
        auto damage = state.damage - targetResult.mitigation;
        targetResult.health.Hurt(Body::Part::Chest, damage);

        ret.back().Adjust(target, targetResult);

        ret.emplace_back(GameChance(game, 1.0-hitChance, L"Miss"));
        ret.back().Adjust(actor, state);
        return ret;
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
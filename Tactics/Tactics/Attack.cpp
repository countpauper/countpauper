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
        GameChances ret;
        ret.emplace_back(GameChance(game, 1.0, L"Hit"));
        ret.back().Adjust(actor, state);
        ret.back().Adjust(target, React(game.Get(target)));
        return ret;
    }

    State Attack::React(const State& state) const
    {
        State result(state);
        result.health.Cut(Sharp(1));
        return result;
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
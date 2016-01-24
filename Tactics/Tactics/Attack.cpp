#include "stdafx.h"
#include "Attack.h"
#include "Game.h"
#include "Actor.h"
#include "Skills.h"
namespace Game
{


    Attack::Attack(Actor& target, const Skill& skill) :
        TargetedAction(target)
    {
        cost = skill.cost;
        range = skill.range;
    }

    State Attack::Act(const State& state, const Game& game)
    {

        State result(state);

        if ((state.mp <= cost) ||
            (state.position.Distance(target.GetPosition()) > range) ||
            (game.Cover(state.position, target.GetPosition())))
        {
            result.possible = false;
            return result;
        }
        result.mp -= cost;
        return result;
    }

    State Attack::React(const State& state) const
    {
        State result(state);
        result.hp -= 1;
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
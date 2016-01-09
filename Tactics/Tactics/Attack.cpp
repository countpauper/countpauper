#include "stdafx.h"
#include "Attack.h"
#include "Game.h"
#include "Actor.h"

namespace Game
{

    Attack::Attack(Actor& target) :
        TargetedAction(target)
    {
        cost = 4;
    }


    State Attack::Act(const State& state, const Game& game)
    {
        State result(state);
        if (state.mp <= cost)
        {
            result.possible = false;
            return result;
        }
        if (state.position.Distance(target.GetPosition()) > 1)
        {
            result.possible = false;
            return result;
        }
        result.mp -= cost;
        return result;
    }

    void Attack::React()
    {
        State result(target);
        result.hp -= 1;
        target.Apply(result);
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

    Slash::Slash(Actor& target) : 
        Attack(target)
    {
    }

} // ::Game
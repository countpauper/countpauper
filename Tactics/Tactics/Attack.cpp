#include "stdafx.h"
#include "Attack.h"
#include "Game.h"
#include "Actor.h"

namespace Game
{

    Attack::Properties::Properties(unsigned cost, float range) :
        Action::Properties(cost),
        range(range)
    {
    }

    Attack::Attack(Actor& target) :
        TargetedAction(target)
    {
    }

    Attack::Properties& Attack::GetAttackProperties()
    {
        return static_cast<Attack::Properties&>(GetProperties());
    }

    State Attack::Act(const State& state, const Game& game)
    {

        State result(state);
        auto properties = GetAttackProperties();
        if ((state.mp <= properties.cost) ||
            (state.position.Distance(target.GetPosition()) > properties.range) ||
            (!game.CanGo(state.position, Direction(target.GetPosition()-state.position))))
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

    Action::Properties& Slash::GetProperties() const
    {
        return properties;
    }
    Attack::Properties Slash::properties(4, 1.0f);


} // ::Game
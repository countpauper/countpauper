#include "stdafx.h"
#include <gl/GL.h>
#include "Move.h"
#include "IGame.h"
#include "Direction.h"
#include "Actor.h"

namespace Game
{

    Move::Move(const Actor& actor, const Position& target, const Skill& skill, Trajectory trajectory) :
		TargetedAction(skill, actor, destination, trajectory),
        destination(target)
    {
    }

    Action::Result  Move::Act(const IGame& game) const
    {
        State state(game.Get(actor));
        if (state.mp < skill.mp)
        {
            return Result();
        }
        state.Spent(skill.mp);
        auto newPosition = destination.GetPosition();
        auto direction = Direction(newPosition - state.position);
        if ((game.CanBe(newPosition)) &&
            (game.CanGo(state.position, direction)))
        {
            state.position = newPosition;
            state.direction = direction;
            Result ret(game, actor);
            ret.state->Adjust(actor, state);
            ret.description = actor.Description() + L" " + skill.name + L" " + direction.Description();
            return ret;
        }
        else
            return Result();
    }


    void Move::Render(const State& state) const
    {
        glColor4ub(255, 255, 255, 255);
        glPushMatrix();
        glTranslatef(float(state.position.x) + 0.5f, 0.5f, float(state.position.y) + 0.5f);
        auto v = destination.GetPosition() - state.position;
        glBegin(GL_LINES);
        glVertex3f(0, 0, 0);
        glVertex3f(float(v.x), 0, float(v.y));
        glEnd();
        glPopMatrix();
    }

    std::wstring Move::Description() const
    {
        return skill.name;
    }

} // ::Game
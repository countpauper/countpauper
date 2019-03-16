#include "stdafx.h"
#include <gl/GL.h>
#include "Move.h"
#include "IGame.h"
#include "Direction.h"
#include "Actor.h"

namespace Game
{

    Move::Move(const Actor& actor, const Position& target, const Skill& skill) :
		TargetedAction(skill, actor, destination),
        destination(target)
    {
    }

    void Move::Act(IGame& game) const
    {
        State state(game.Get(actor));
        state.Spent(skill.mp);
        auto newPosition = destination.GetPosition();
        auto direction = Direction(newPosition - state.position);
        state.position = newPosition;
        state.direction = direction;
        game.Adjust(actor, state, skill.name + L" " + direction.Description());
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
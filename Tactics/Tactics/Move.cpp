#include "stdafx.h"
#include <gl/GL.h>
#include "Move.h"
#include "Game.h"
#include "Game/Direction.h"
#include "Actor.h"

namespace Game
{

    Move::Move(const Actor& actor, const Position& destination, const Skill& skill) :
        Action(skill, actor),
        destination(destination)
    {
    }

    std::unique_ptr<GameState> Move::Act(const IGame& game) const
    {
        State state(game.Get(actor));
        if (state.mp <= skill.mp)
        {
            return nullptr;
        }
        state.mp -= skill.mp;
        auto newPosition = destination;
        auto direction = Direction(newPosition - state.position);
        if ((game.CanBe(newPosition)) &&
            (game.CanGo(state.position, direction)))
        {
            state.position = newPosition;
            state.direction = direction;
            auto ret = std::make_unique<GameState>(game, actor);
            ret->Adjust(actor, state);
            return ret;
        }
        else
            return nullptr;
    }

    void Move::Render(const State& state) const
    {
        glColor4ub(255, 255, 255, 255);
        glPushMatrix();
        glTranslatef(float(state.position.x) + 0.5f, 0.5f, float(state.position.y) + 0.5f);
        auto v = destination - state.position;
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
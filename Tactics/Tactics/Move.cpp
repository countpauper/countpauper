#include "stdafx.h"
#include <gl/GL.h>
#include "Move.h"
#include "Game.h"
#include "Direction.h"

namespace Game
{

    Move::Move(const Actor& actor, Direction direction) :
        Action(actor),
        direction(direction)
    {
    }

    std::unique_ptr<GameState> Move::Act(const IGame& game) const
    {
        auto cost = 2;  // TODO move skills
        auto& actor = *game.ActiveActor();
        State state(game.Get(actor));
        if (state.mp <= 2)
        {
            return nullptr;
        }
        state.mp -= cost;
        auto newPosition = state.position + direction.Vector();
        if ((game.CanBe(newPosition)) &&
            (game.CanGo(state.position, direction)))
        {
            state.position = newPosition;
            state.direction = direction;
            auto ret = std::make_unique<GameState>(game);
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
        auto v = direction.Vector();
        glBegin(GL_LINES);
        glVertex3f(0, 0, 0);
        glVertex3f(float(v.x), 0, float(v.y));
        glEnd();
        glPopMatrix();
    }

    std::wstring Move::Description() const
    {
        return direction.Description();
    }

    North::North(const Actor& actor) :
        Move(actor, Direction::Value::North)
    {
    }


    East::East(const Actor& actor) :
        Move(actor, Direction::Value::East)
    {
    }

    South::South(const Actor& actor) :
        Move(actor, Direction::Value::South)
    {
    }
    West::West(const Actor& actor) :
        Move(actor, Direction::Value::West)
    {
    }

} // ::Game
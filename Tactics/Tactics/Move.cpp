#include "stdafx.h"
#include <gl/GL.h>
#include "Move.h"
#include "Game.h"
#include "Direction.h"

namespace Game
{

    Move::Move(Direction direction) :
        direction(direction)
    {
    }

    GameChances Move::Act(IGame& game)
    {
        auto cost = 2;  // TODO move skills
        auto& actor = *game.ActiveActor();
        State state(game.Get(actor));
        if (state.mp <= 2)
        {
            return GameChances();
        }
        state.mp -= cost;
        auto newPosition = state.position + direction.Vector();
        if ((game.CanBe(newPosition)) && 
            (game.CanGo(state.position, direction)))
        {
            state.position = newPosition;
            state.direction = direction;
            GameChances ret;
            ret.emplace_back(GameChance(game, 1.0, L"Move"));
            ret.back()->Adjust(actor, state);
            return ret;
        }
        else
            return GameChances();
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

    North::North() :
        Move(Direction::Value::North)
    {
    }


    East::East() :
        Move(Direction::Value::East)
    {
    }

    South::South() :
        Move(Direction::Value::South)
    {
    }
    West::West() :
        Move(Direction::Value::West)
    {
    }

} // ::Game
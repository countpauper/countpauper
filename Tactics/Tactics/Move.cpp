#include "stdafx.h"
#include "Move.h"
#include "Game.h"
#include "Direction.h"

namespace Game
{

    Move::Move(Direction direction) :
        direction(direction)
    {
    }

    State Move::Act(const State& state, const Game& game)
    {
        State result(state);
        if (state.mp <= cost)
        {
            result.possible = false;
            return result;
        }
        result.mp -= cost;
        result.position += direction.Vector();
        result.possible = game.CanBe(result.position) &&
            game.CanGo(state.position, direction);
        result.direction = direction;
        return result;
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

    
    Action::Properties Move::properties(2);

    Action::Properties& Move::GetProperties() const
    {
        return properties;
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
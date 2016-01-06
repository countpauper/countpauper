#include "stdafx.h"
#include <gl/GL.h>
#include "Actor.h"
#include "Action.h"
#include "Color.h"

namespace Game
{
    Object::Object() :
        hp(0),
        maxhp(0)
    {

    }
    void Object::Move(int dx, int dy)
    {
        position.x = std::min(std::max(0,static_cast<int>(position.x) + dx),5);
        position.y = std::min(std::max(0,static_cast<int>(position.y) + dy),5);
    }

    Position Object::GetPosition() const
    {
        return position;
    }

    unsigned Object::GetHitPoints() const
    {
        return hp;
    }

    Actor::Actor() :
        mp(0),
        maxmp(0),
        team(0)
    {
    }

    void Actor::Render() const
    {
        unsigned sides = 16;
        float r = 0.25f;

        Engine::RGBA teamColor[] = {
            { 255, 0, 0, 255 },
            { 0, 255, 0, 255 },
            { 0, 0, 255, 255 }
        };
        const float HALF_PI = M_PI*0.5f;
        glPushMatrix();
        if (hp <= 0)
            glRotatef(90, 1, 0, 0);
        glRotatef(Rad2Deg(direction.Angle()), 0, 1, 0);
        glBegin(GL_TRIANGLES);
        for (unsigned i = 0; i < sides; ++i)
        {
            float a = float(i) / sides * 2.0f * M_PI;
            float b = float(i + 1) / sides * 2.0f * M_PI;
            teamColor[team].Render();
            
            float xa = cos(a) * r;
            float ya = sin(a) * sin(a*0.5f) * r;
            float xb = cos(b) * r;
            float yb =  sin(b) * sin(b*0.5f) * r;

            glNormal3f(0.0f, -1.0f, 0.0f);
            glVertex3f(0, 0, 0);
            glVertex3f(xb, 0, yb);
            glVertex3f(xa, 0, ya);

            glNormal3f(cos(a), 0.0f, sin(a));
            glVertex3f(xa, 1, ya);
            glVertex3f(xa, 0, ya);
            glNormal3f(cos(b), 0.0f, sin(b));
            glVertex3f(xb, 0, yb);

            glNormal3f(cos(a), 0.0f, sin(a));
            glVertex3f(xa, 1, ya);
            glNormal3f(cos(b), 0.0f, sin(b));
            glVertex3f(xb, 0, yb);
            glVertex3f(xb,    1,    yb);

            glNormal3f(0.0f, 1.0f, 0.0f);
            glVertex3f(0, 1, 0);
            glVertex3f(xa, 1, ya);
            glVertex3f(xb, 1, yb);
        }
        glEnd();
        glPopMatrix();
    }

    unsigned Actor::GetMovePoints() const
    {
        return mp;
    }

    Direction Actor::GetDirection() const
    {
        return direction;
    }
    void Actor::Apply(const State& result)
    {
        if (!result.possible)
            return;
        position = result.position;
        direction = result.direction;
        mp = result.mp;
        hp = result.hp;
    }

    void Actor::Turn()
    {
        mp = maxmp;
    }
    std::wistream& operator>>(std::wistream& s, Actor& actor)
    {
        s >> actor.position.x >> actor.position.y;
        s >> actor.team;
        s >> actor.hp >> actor.maxhp;
        s >> actor.mp >> actor.maxmp;
        return s;
    }
} // ::Game
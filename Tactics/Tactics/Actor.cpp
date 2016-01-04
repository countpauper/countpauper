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
        position.x = min(max(0,static_cast<int>(position.x) + dx),5);
        position.y = min(max(0,static_cast<int>(position.y) + dy),5);
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
        float r = 0.25;

        RGBA teamColor[] = {
            { 255, 0, 0, 255 },
            { 0, 255, 0, 255 },
            { 0, 0, 255, 255 }
        };

        if (hp == 0)
            glRotatef(90, 1, 0, 0);
        glBegin(GL_TRIANGLES);
        for (unsigned i = 0; i < sides; ++i)
        {
            float a = float(i) / sides * 2.0f * M_PI;
            float b = float(i + 1) / sides * 2.0f * M_PI;
            float ra = r;
            float rb = r;
            float da = std::fabs(ShortestTurn(direction.Angle(),a));
            auto color = teamColor[team] * (1.0f-da / M_PI); // ghetto shading
            color.Render();

            const float HALF_PI = M_PI*0.5f;
            if (da < M_PI*0.5)
            {
                ra += 0.25f*(HALF_PI - da)/HALF_PI;
            }
            float xa =  cos(a)*ra;
            float ya =  sin(a) * ra;
            float db = std::fabs(ShortestTurn(direction.Angle(),b));
            if (db < M_PI*0.5)
            {
                rb += 0.25f*(HALF_PI - db)/HALF_PI;
            }
            float xb =  cos(b)*rb;
            float yb =  sin(b) * rb;

            glNormal3f(0.0f, -1.0f, 0.0f);
            glVertex3f(0, 0, 0);
            glVertex3f(xa, 0, ya);
            glVertex3f(xb,    0,    yb);

            glNormal3f(cos(a), 0.0f, sin(a));
            glVertex3f(xa, 1, ya);
            glVertex3f(xa, 0, ya);
            glVertex3f(xb,  0,    yb);

            glVertex3f(xa, 1, ya);
            glVertex3f(xb, 0, yb);
            glVertex3f(xb,    1,    yb);

            glNormal3f(0.0f, 1.0f, 0.0f);
            glVertex3f(0, 1, 0);
            glVertex3f(xa, 1, ya);
            glVertex3f(xb,    1,    yb);

        }
        glEnd();
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
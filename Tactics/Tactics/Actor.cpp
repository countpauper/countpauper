#include "stdafx.h"
#include "Actor.h"
#include <gl/GL.h>

namespace Game
{
    void Actor::Render() const
    {
        unsigned sides = 10;
        float x = static_cast<float>(position.x)+0.5f;
        float z = static_cast<float>(position.y) + 0.5f;
        float r = 0.25;
        glColor3f(1.0f, 0.0f, 0.0f);
        glBegin(GL_TRIANGLES);
        for (unsigned i = 0; i < sides; ++i)
        {
            float a = static_cast<float>(i) / sides * 2 * M_PI;
            float b = static_cast<float>(i + 1) / sides * 2 * M_PI;
            float xa = x + cos(a)*r;
            float za = z + sin(a) * r;
            float xb = x + cos(b)*r;
            float zb = z + sin(b) * r;

            glVertex3f(x, 0, z);
            glVertex3f(xa, 0, za);
            glVertex3f(xb, 0, zb);

            glVertex3f(xa, 0, za);
            glVertex3f(xb, 0, zb);
            glVertex3f(xa, 1, za);

            glVertex3f(xb, 0, zb);
            glVertex3f(xa, 1, za);
            glVertex3f(xb, 1, zb);

            glVertex3f(x, 1, z);
            glVertex3f(xa, 1, za);
            glVertex3f(xb, 1, zb);

        }
        glEnd();
    }

    std::wistream& operator>>(std::wistream& s, Actor& actor)
    {
        s >> actor.position.x >> actor.position.y;
        return s;
    }
} // ::Game
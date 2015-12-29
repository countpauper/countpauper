#include "stdafx.h"
#include <gl/GL.h>
#include "Actor.h"
#include "Action.h"
#include "Color.h"

namespace Game
{
    void Object::Move(int dx, int dy)
    {
        position.x = min(max(0,static_cast<int>(position.x) + dx),5);
        position.y = min(max(0,static_cast<int>(position.y) + dy),5);
    }

	Position Object::GetPosition() const
	{
		return position;
	}
	Actor::Actor() :
		actionPoints(10),
		team(0)
	{
	}


	void Actor::Render(float z) const
    {
        unsigned sides = 10;
        float x = static_cast<float>(position.x)+0.5f;
        float y = static_cast<float>(position.y) + 0.5f;
        float r = 0.25;

		RGBA teamColor[] = {
			{ 255, 0, 0, 255 },
			{ 0, 255, 0, 255 },
			{ 0, 0, 255, 255 }
		};
        glColor3f(1.0f, 0.0f, 0.0f);
        glBegin(GL_TRIANGLES);
        for (unsigned i = 0; i < sides; ++i)
        {
			teamColor[team].Render();
            float a = static_cast<float>(i) / sides * 2.0f * static_cast<float>(M_PI);
			float b = static_cast<float>(i + 1) / sides * 2.0f * static_cast<float>(M_PI);
            float xa = x + cos(a)*r;
            float ya = y + sin(a) * r;
            float xb = x + cos(b)*r;
            float yb = y + sin(b) * r;

            glVertex3f(x,	z,		y);
            glVertex3f(xa,	z,		ya);
            glVertex3f(xb,	z,		yb);

            glVertex3f(xa,	z,		ya);
            glVertex3f(xa,	z+1,	ya);
			glVertex3f(xb, z, yb);

            glVertex3f(xb,	z,		yb);
            glVertex3f(xa,	z+1,	ya);
            glVertex3f(xb,	z+1,	yb);

			glVertex3f(x,	z + 1,	y);
			glVertex3f(xa,	z + 1,	ya);
            glVertex3f(xb,	z+1,	yb);

        }
        glEnd();
    }

	unsigned Actor::GetActionPoints() const
	{
		return actionPoints;
	}

	void Actor::Apply(const Result& result)
	{
		if (!result.possible)
			return;
		position = result.position;
		actionPoints = result.actionPoints;
	}

    std::wistream& operator>>(std::wistream& s, Actor& actor)
    {
        s >> actor.position.x >> actor.position.y;
		s >> actor.team;
        return s;
    }
} // ::Game
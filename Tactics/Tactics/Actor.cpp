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

	void Actor::Render() const
    {
        unsigned sides = 10;
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
            float xa =  cos(a)*r;
            float ya =  sin(a) * r;
            float xb =  cos(b)*r;
            float yb =  sin(b) * r;

            glVertex3f(0,	0,	0);
            glVertex3f(xa,	0,	ya);
            glVertex3f(xb,	0,	yb);

            glVertex3f(xa,	0,	ya);
            glVertex3f(xa,	1,	ya);
			glVertex3f(xb,  0,	yb);

            glVertex3f(xb,	0,	yb);
            glVertex3f(xa,	1,	ya);
            glVertex3f(xb,	1,	yb);

			glVertex3f(0,	1,	0);
			glVertex3f(xa,	1,	ya);
            glVertex3f(xb,	1,	yb);

        }
        glEnd();
    }

	unsigned Actor::GetActionPoints() const
	{
		return actionPoints;
	}

	void Actor::Apply(const State& result)
	{
		if (!result.possible)
			return;
		position = result.position;
		actionPoints = result.actionPoints;
	}

	void Actor::Turn()
	{
		actionPoints = 10;
	}
    std::wistream& operator>>(std::wistream& s, Actor& actor)
    {
        s >> actor.position.x >> actor.position.y;
		s >> actor.team;
        return s;
    }
} // ::Game
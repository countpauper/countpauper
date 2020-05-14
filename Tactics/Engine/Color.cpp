#include "stdafx.h"
#include "Color.h"
#include <cmath>
#include <gl/GL.h>

namespace Engine
{

    void RGBA::Render() const
    {
        glColor4ub(r, g, b, a);

        //GLfloat color[] = { r, g, b, a };
        //glColor4fv(color);
        //glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, diffuse);
        //glMaterialf(GL_FRONT, GL_SHININESS, 50.0);
    }

    RGBA RGBA::operator*=(double factor)
    {
        if (factor > 0)
        {
            r = GLubyte(std::min(255.0, round(factor* double(r))));
            g = GLubyte(std::min(255.0, round(factor* double(g))));
            b = GLubyte(std::min(255.0, round(factor* double(b))));
        }
        else
        {
            r = g = b = 0;
        }
        return *this;
    }

    RGBA RGBA::Translucent(double factor) const
    {
        RGBA result(*this);
        if (factor > 0)
            result.a = GLubyte(std::min(255.0, round(factor * float(a))));
        else
            result.a = 0;
        return result;
    }

    RGBA operator*(const RGBA& color, double factor)
    {
        RGBA result(color);
        return result *= factor;
    }

    bool RGBA::IsVisible() const
    {
        return a > 0;
    }

    bool RGBA::IsOpaque() const
    {
        return a == 255;
    }


    RGBA RGBA::transparent(0, 0, 0, 0);
    RGBA RGBA::white(255, 255, 255);
    RGBA RGBA::black(0, 0, 0);
    RGBA RGBA::red(255, 0, 0);
    RGBA RGBA::green(0, 255, 0);
    RGBA RGBA::blue(0, 0, 255);

}   // ::Engine
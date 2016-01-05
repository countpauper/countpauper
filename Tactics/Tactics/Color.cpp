#include "stdafx.h"
#include "Color.h"
#include <cmath>

void RGBA::Render() const
{
    glColor4ub(r, g, b, a);

    //GLfloat color[] = { r, g, b, a };
    //glColor4fv(color);
    //glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, diffuse);
    //glMaterialf(GL_FRONT, GL_SHININESS, 50.0);
}

RGBA RGBA::operator*=(float factor)
{
    r = GLubyte(round(factor* float(r)));
    g = GLubyte(round(factor* float(g)));
    b = GLubyte(round(factor* float(b)));
    return *this;
}

RGBA operator*(const RGBA& color, float factor)
{
    RGBA result(color);
    return result *= factor;
}


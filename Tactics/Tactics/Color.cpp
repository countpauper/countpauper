#include "stdafx.h"
#include "Color.h"
#include <cmath>

void RGBA::Render() const
{
    glColor4ub(r, g, b, a);
}
RGBA RGBA::operator*=(float factor)
{
    r = static_cast<GLubyte>(round(factor* static_cast<float>(r)));
    g = static_cast<GLubyte>(round(factor* static_cast<float>(g)));
    b = static_cast<GLubyte>(round(factor* static_cast<float>(b)));
    return *this;
}

RGBA operator*(const RGBA& color, float factor)
{
    RGBA result(color);
    return result *= factor;
}


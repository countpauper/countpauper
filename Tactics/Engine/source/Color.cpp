#include "Engine/Color.h"
#include <cmath>
#include <GL/gl.h>

namespace Engine
{

RGBA::RGBA(Component r, Component g, Component b, Component a) :
    r(r),
    g(g),
    b(b),
    a(a)
{
}

RGBA::RGBA(uint32_t c) :
    r(c & 0xFF),
    g((c >> 8) & 0xFF),
    b((c >> 16) & 0xFF),
    a((c >> 24))
{
}

void RGBA::Render() const
{
    glColor4ub(r, g, b, a);

    //GLfloat color[] = { r, g, b, a };
    //glColor4fv(color);
    //glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, diffuse);
    //glMaterialf(GL_FRONT, GL_SHININESS, 50.0);
}

bool RGBA::operator==(RGBA other) const
{
    return r==other.r && g==other.g && b==other.b && a==other.a;
}

RGBA RGBA::operator*=(double factor)
{
    if (factor > 0)
    {
        a = std::min(255, int(std::round(factor * a)));
    }
    else
    {
        a = 0;
    }
    return *this;
}

RGBA RGBA::operator+=(RGBA other)
{
    Component newAlpha = std::min(255, a + other.a);
    double wa, wb;
    if (newAlpha> 0 ) {
        wa = a / double(newAlpha);
        wb = other.a / double(newAlpha);
    }
    else
    {
        wa = wb = 0.5;
    }

    r = GLubyte(std::min(255.0, (r*wa) + (other.r*wb)));
    g = GLubyte(std::min(255.0, (g*wa) + (other.g*wb)));
    b = GLubyte(std::min(255.0, (b*wa) + (other.b*wb)));
    a = GLubyte(newAlpha);
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

RGBA operator*(RGBA color, double factor)
{
    RGBA result(color);
    return result *= factor;
}

RGBA operator+(RGBA a, RGBA b)
{
    RGBA result(a);
    return result += b;
}

bool RGBA::IsVisible() const
{
    return a > 0;
}

bool RGBA::IsOpaque() const
{
    return a == 255;
}


RGBA RGBA::transparent{0, 0, 0, 0};
RGBA RGBA::white{255, 255, 255};
RGBA RGBA::black{0, 0, 0};
RGBA RGBA::red{255, 0, 0};
RGBA RGBA::green{0, 255, 0};
RGBA RGBA::blue{0, 0, 255};

HSVA::HSVA(Component h, Component s, Component v, Component a) :
    h(h),
    s(s),
    v(v),
    a(a)
{
}

HSVA::HSVA(RGBA rgba)
{
    a = rgba.a;
    v = std::max(std::max(rgba.r, rgba.g), rgba.b);
    Component chroma = v - std::min(std::min(rgba.r, rgba.g),rgba.b);
    float sixth = 256.0/6;  // 60 deg on the 360 deg color wheel

    if (chroma==0)
    {
        h = 0;
    }
    else if (v==rgba.r)
    {
        h = std::round(sixth * ((rgba.g - rgba.b) / chroma));
    }
    else if (v==rgba.g)
    {
        h = std::round(sixth * (rgba.b - rgba.r) / chroma + 2 * sixth);
    }
    else // if (v == rgba.b)
    {
        h = std::round(sixth * (rgba.r - rgba.g) / chroma + 4 * sixth);
    }
    if (v==0)
    {
        s = 0;
    }
    else
    {
        s = int(255 * chroma) / v;
    }
}


bool HSVA::operator==(HSVA other) const
{
    return h==other.h && s==other.s && v==other.v && a==other.a;
}

}   // ::Engine

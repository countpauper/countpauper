#pragma once
#include <gl/GL.h>

namespace Engine
{
    struct RGBA
    {
        RGBA(GLubyte r, GLubyte g, GLubyte b, GLubyte a) :
            r(r), g(g), b(b), a(a)
        {
        }
        GLubyte r, g, b, a;
        void Render() const;
        RGBA operator*=(float factor);
    };

    RGBA operator*(const RGBA& color, float factor);

}   // ::Engine
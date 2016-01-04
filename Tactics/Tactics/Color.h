#pragma once
#include <gl/GL.h>

struct RGBA 
{ 
    GLubyte r, g, b, a; 
    void Render() const;
    RGBA operator*=(float factor);
};

RGBA operator*(const RGBA& color, float factor);

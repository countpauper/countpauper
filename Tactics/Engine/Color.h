#pragma once
#include <cstdint>

namespace Engine
{
    struct RGBA
    {
        using Component = uint8_t;

        RGBA(Component r, Component g, Component b, Component a) :
            r(r), g(g), b(b), a(a)
        {
        }
        Component r, g, b, a;
        void Render() const;
        RGBA operator*=(float factor);
    };

    RGBA operator*(const RGBA& color, float factor);

}   // ::Engine
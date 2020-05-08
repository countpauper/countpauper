#pragma once
#include <cstdint>

namespace Engine
{
    struct RGBA
    {
        using Component = uint8_t;
        RGBA() = default;

        RGBA(Component r, Component g, Component b, Component a=255) :
            r(r), g(g), b(b), a(a)
        {
        }
        RGBA(uint32_t c) :
            r(c & 0xFF),
            g((c >> 8) & 0xFF),
            b((c >> 16) & 0xFF),
            a((c >> 24))
        {
        }
        Component r, g, b, a;
        void Render() const;
        bool IsVisible() const;
        operator bool() const { return IsVisible(); }
        RGBA operator*=(double factor);
        RGBA Translucent(double factor) const;

        static RGBA transparent;
        static RGBA white;
        static RGBA black;
        static RGBA red;
        static RGBA green;
        static RGBA blue;
    };

    RGBA operator*(const RGBA& color, double factor);

}   // ::Engine
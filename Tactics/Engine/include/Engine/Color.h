#pragma once
#include <cstdint>

namespace Engine
{



    struct RGBA
    {
        using Component = uint8_t;
        RGBA(Component r, Component g, Component b, Component a = 255);
        explicit RGBA(uint32_t c);
        void Render() const;
        bool IsVisible() const;
        bool IsOpaque() const;
        explicit operator bool() const { return IsVisible(); }
        bool operator==(RGBA other) const;
        RGBA operator*=(double factor);
        RGBA operator+=(RGBA other);
        RGBA Translucent(double factor) const;

        Component r, g, b, a;

        static RGBA transparent;
        static RGBA white;
        static RGBA black;
        static RGBA red;
        static RGBA green;
        static RGBA blue;
    };

    RGBA operator*(RGBA color, double factor);
    RGBA operator+(RGBA a, RGBA b);


    struct HSVA
    {
        using Component = uint8_t;
        Component h = 0, s = 0, v = 0, a = 0;
        HSVA(Component h, Component s, Component v, Component a = 255);
        explicit HSVA(RGBA rgba);
        bool operator==(HSVA other) const;

    };
}   // ::Engine

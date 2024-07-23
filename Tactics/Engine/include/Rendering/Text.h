#pragma once

#include <string>

namespace Engine
{
    class Font
    {
    public:
        Font();
        void Select() const;
        unsigned Height() const;
        static Font basic;
    private:
        unsigned base;
        unsigned height;
        static unsigned nextBase;
    };

    enum class Align
    {
        bottom=-1,
        right=-1,
        center=0,
        left=1,
        top=1
    };

    void glText(std::string_view, Align horizontal = Align::left, Align vertical = Align::top);
}

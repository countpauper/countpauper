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


    void glText(const std::string& text);
}

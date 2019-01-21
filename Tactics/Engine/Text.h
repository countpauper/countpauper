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
        static Font& default;
    private:
        unsigned base;
        unsigned height;
        static unsigned nextBase;
    };


    void glText(const std::string& text);
    void glText(const std::wstring& text);
}
#pragma once

#include <string>

namespace Engine
{
    class Font
    {
    public:
        Font();
        void Select() const;
        static Font& system;
    private:
        unsigned base;
        static unsigned nextBase;
    };


    void glText(const std::string& text);
    void glText(const std::wstring& text);
}

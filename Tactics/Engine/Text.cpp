#include "stdafx.h"
#include "Text.h"
#include <gl/GL.h>

namespace Engine
{
 
    unsigned Font::nextBase = 1000;
    Font& Font::system = Font();

    Font::Font() :
        base(nextBase)
    {
        auto hdc = GetDC(nullptr);
        SelectObject(hdc, GetStockObject(SYSTEM_FONT));
        wglUseFontBitmaps(hdc, 0, 255, base);
        nextBase += 256;    // TODO can be optimized to 100 main characters (32 to 128-32)
        ReleaseDC(nullptr, hdc);
    }

    void Font::Select() const
    {
        glListBase(base);
    }

    void glText(const std::string& text)
    {
        glRasterPos3i(0, 0, 1); // set start position
        glCallLists(text.size(), GL_UNSIGNED_BYTE, text.c_str());
    }

    void glText(const std::wstring& wtext)
    {
        std::string text(wtext.begin(), wtext.end());
        glText(text);
    }
}

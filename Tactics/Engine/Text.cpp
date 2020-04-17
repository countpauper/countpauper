#include "stdafx.h"
#include "Text.h"
#include "from_string.h"
#include <gl/GL.h>
#include <algorithm>

namespace Engine
{
 
    unsigned Font::nextBase = 1000;
    Font& Font::default = Font();

    Font::Font() :
        base(nextBase),
        height(16)
    {
        auto hdc = GetDC(nullptr);
        HFONT font = CreateFont(height, 0, 0, 0,
            FW_NORMAL, FALSE, FALSE, FALSE,
            ANSI_CHARSET, 0,
            0, 0, 0, L"Arial"); // can only use true type fonts 
        //  this one is 40 points in size
        SelectObject(hdc, font);

        wglUseFontBitmaps(hdc, 0, 127, base);
        nextBase += 128;    // TODO can be optimized to 100 main characters (32 to 128-32)
        ReleaseDC(nullptr, hdc);
    }

    void Font::Select() const
    {
        glListBase(base);
    }

    unsigned Font::Height() const
    {
        return height;
    }

    void glText(const std::string& text)
    {
        glRasterPos3i(0, 0, 1); // set start position
        glCallLists(static_cast<GLsizei>(std::min({ 65535ull, text.size() })), GL_UNSIGNED_BYTE, text.c_str());
    }

    void glText(const std::wstring& text)
    {
        glText(from_string<std::string>(text));
    }
}

#include "Rendering/Text.h"
#include <GL/gl.h>
#include <GL/glut.h>
#include <cassert>
#include <algorithm>
#include "UI/Window.h"

namespace Engine
{

    unsigned Font::nextBase = 1000;
    Font Font::basic;

    Font::Font() :
        base(nextBase),
        height(16)
    {
        #ifdef WIN32
        auto hdc = GetDC(nullptr);
        HFONT font = CreateFont(height, 0, 0, 0,
            FW_NORMAL, FALSE, FALSE, FALSE,
            ANSI_CHARSET, 0,
            0, 0, 0, "Arial"); // can only use true type fonts
        //  this one is 40 points in size
        SelectObject(hdc, font);

        wglUseFontBitmaps(hdc, 0, 127, base);
        nextBase += 128;    // TODO can be optimized to 100 main characters (32 to 128-32)
        ReleaseDC(nullptr, hdc);
        #else
        // nothing needed it seems
        #endif
    }

    void Font::Select() const
    {
        glListBase(base);
    }

    unsigned Font::Height() const
    {
        return height;
    }

    void glText(std::string_view text, Align horizontal_align, Align vertical_align)
    {
        assert(horizontal_align == Align::left);  // only supported
        assert(vertical_align != Align::center);   // only supported;

        auto inverseScale = Engine::Matrix::ModelView().SetTranslation(Vector::zero).Inverse();

        auto pixel = Window::CurrentWindow()->PixelScale();
        pixel *= inverseScale;

        float lineHeight = 24.0 * pixel.y;
        //glDisable(GL_TEXTURE_2D);
        double y = 0;
        if (vertical_align == Align::bottom)
        {
            auto lines = std::count(text.begin(), text.end(), '\n');
            y = 1.0 - (lines - 1) * lineHeight;
        }
        else if (vertical_align == Align::top)
        {
            y = lineHeight * 1;
        }

        glBindTexture(GL_TEXTURE_2D,0);
        glRasterPos3f(0, y, 0); // set start position
        for(auto c : text)
        {
            if (c=='\n') {
                y += lineHeight;
                glRasterPos3f(0, y, 0);
            }
            else if ((y>=0.0) && (y<=1.0))  // crude clipping
                glutBitmapCharacter( GLUT_BITMAP_TIMES_ROMAN_24, c);
        }
    }

}

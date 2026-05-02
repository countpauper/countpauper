#include "Rendering/Text.h"
#include "Utility/Assert.h"
#include "Geometry/Matrix.h"
#include "Geometry/Vector.h"
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <algorithm>
#include <utility>
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

        // Compute the start point in window coordinates directly, so raster position
        // is not rejected by the current projection/modelview when text is partially off-screen.
        GLint viewport[4];
        glGetIntegerv(GL_VIEWPORT, viewport);

        auto modelView = Matrix::ModelView();
        auto projection = Matrix::Projection();
        auto transform = projection * modelView;

        auto projectWindow = [&](const Vector& o) -> std::pair<int, int>
        {
            auto ndc = transform * o;
            int winX = viewport[0] + static_cast<int>(0.5 * (ndc.X() + 1.0) * viewport[2]);
            int winY = viewport[1] + static_cast<int>(0.5 * (ndc.Y() + 1.0) * viewport[3]);
            return {winX, winY};
        };

        auto pixel = Window::CurrentWindow()->PixelScale();
        float lineHeightPixel = 24.0f; // TODO: get from current font

        float yorigin = 0;
        float lineshift = -1;
        if (vertical_align == Align::bottom)
        {
            yorigin = 1.0;
            lineshift = std::count(text.begin(), text.end(), '\n') - 1;
        }
        else if (vertical_align == Align::center)
        {
            yorigin = 0,5;
            lineshift = std::count(text.begin(), text.end(), '\n') / 2.0;
        }
        auto [winX, winY] = projectWindow({0.0, yorigin, 0.0});
        winY += lineshift * lineHeightPixel;

        glBindTexture(GL_TEXTURE_2D, 0);

        glWindowPos2i(winX, winY);

        for (auto c : text)
        {
            if (c == '\n')
            {
                winY -= lineHeightPixel;
                glWindowPos2i(winX, winY);
            }
            else
            {
                glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
            }
        }
    }

}

#include "Rendering/Text.h"
#include "Rendering/Font.h"
#include "Utility/Assert.h"
#include "Geometry/Matrix.h"
#include "Geometry/Vector.h"
#include "UI/Window.h"
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glut.h>

namespace Engine
{



void glStrokeText(std::string_view text, const Font& font, Align horizontal_align, Align vertical_align)
{
    assert(horizontal_align == Align::left);  // only supported
    // Stroke fonts: render in model space using the current matrix
    float yorigin = 0.0;
    float lineshift = -1;
    if (vertical_align == Align::bottom)
    {
        yorigin = 1.0;
        lineshift = std::count(text.begin(), text.end(), '\n') - 1;
    }
    else if (vertical_align == Align::center)
    {
        yorigin = 0.5;
        lineshift = (std::count(text.begin(), text.end(), '\n')+1) / -2.0f;
    }

    auto scale = Window::CurrentWindow()->PixelScale();
    auto pixelAspect = scale.X() / scale.Y();
    glPushMatrix();
    glTranslatef(0.0f, yorigin - lineshift * font.Height(), 0.0f);
    glScaled(pixelAspect, -1.0, 1);


    float carriageReturn = 0.0;
    for (auto c : text)
    {
        if (c == '\n')
        {
            glTranslatef(-carriageReturn, -font.Height(), 0.0f);
            carriageReturn = 0.0f;
        }
        else
        {
            font.Render(c);
            carriageReturn += font.Width(std::string_view(&c, 1));
        }
    }
    glPopMatrix();
}

void glRasterText(std::string_view text, const Font& font, Align horizontal_align, Align vertical_align)
{
    // Bitmap fonts: render in window space via projected raster position
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

    float yorigin = 0;
    float lineshift = -1;
    if (vertical_align == Align::bottom)
    {
        yorigin = 1.0;
        lineshift = std::count(text.begin(), text.end(), '\n') - 1;
    }
    else if (vertical_align == Align::center)
    {
        yorigin = 0.5;
        lineshift = std::count(text.begin(), text.end(), '\n') / 2.0f;
    }
    auto [winX, winY] = projectWindow({0.0, yorigin, 0.0});
    winY += static_cast<int>(lineshift * font.Height());

    glBindTexture(GL_TEXTURE_2D, 0);

    glWindowPos2i(winX, winY);

    for (auto c : text)
    {
        if (c == '\n')
        {
            winY -= static_cast<int>(font.Height());
            glWindowPos2i(winX, winY);
        }
        else
        {
            font.Render(c);
        }
    }
}

void glText(std::string_view text, const Font& font, Align horizontal_align, Align vertical_align)
{
    if (font.IsStroke())
    {
        glStrokeText(text, font, horizontal_align,  vertical_align);

    }
    else
    {
        glRasterText(text, font, horizontal_align, vertical_align);
    }
}


}

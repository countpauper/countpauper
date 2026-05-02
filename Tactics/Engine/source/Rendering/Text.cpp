#include "Rendering/Text.h"
#include "Rendering/Font.h"
#include "Utility/Assert.h"
#include "Geometry/Matrix.h"
#include "Geometry/Vector.h"
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glut.h>

namespace Engine
{


void glText(std::string_view text, const Font& font, Align horizontal_align, Align vertical_align)
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
    winY += lineshift * font.Height();

    glBindTexture(GL_TEXTURE_2D, 0);

    glWindowPos2i(winX, winY);

    for (auto c : text)
    {
        if (c == '\n')
        {
            winY -=  font.Height();
            glWindowPos2i(winX, winY);
        }
        else
        {
            font.Render(c);
        }
    }
}

}

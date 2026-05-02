#include "Rendering/Font.h"
#include "Utility/Assert.h"
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <stdexcept>
#include <numeric>
#include <format>


namespace Engine
{

struct FontDescriptor
{
    std::string_view name;
    unsigned height;    // if 0 then scalable (not supported yet)
    void* ptr;
};

FontDescriptor glutFonts[]={
    {"stroke roman", 0,  GLUT_STROKE_ROMAN},
    {"stroke mono roman", 0,  GLUT_STROKE_MONO_ROMAN},
    {"mono", 15,  GLUT_BITMAP_9_BY_15},
    {"mono", 13, GLUT_BITMAP_8_BY_13} ,
    {"times roman", 10, GLUT_BITMAP_TIMES_ROMAN_10},
    {"times roman", 24, GLUT_BITMAP_TIMES_ROMAN_24},
    {"helvetica", 10, GLUT_BITMAP_HELVETICA_10},
    {"helvetica", 12, GLUT_BITMAP_HELVETICA_12},
    {"helvetica", 18, GLUT_BITMAP_HELVETICA_18}
};

FontDescriptor* FindFont(std::string_view name, unsigned height)
{
    for(auto& font : glutFonts)
    {
        if ((font.name == name) &&
            ((font.height == 0) || (font.height == height)))
            return &font;
    }
    return nullptr;
}

Font::Font(std::string_view name, unsigned height) :
    font(FindFont(name, height)),
    height(height)
{
    if (!font)
        throw std::invalid_argument(std::format("Unknown font: {} {}", name, height));
    assert(font->height);   // Stroke fonts not supported yet
}

unsigned Font::Height(std::string_view text) const
{
    if (!font)
        return 0;
    unsigned lines = std::count(text.begin(), text.end(), '\n') + 1;
    if (font->height)
        return font->height * lines;
    else
        return height * lines;
}

unsigned Font::Width(std::string_view text) const
{
    unsigned sum = 0;
    if (!font)
        return sum;
    else if (font->height == 0)
    {
        return std::accumulate(text.begin(), text.end(), 0, [&](unsigned sum, char c)
        {
            assert(c!='\n');    // TODO: split lines and compute maximum
            return sum + glutStrokeWidth(font->ptr, c);
        });
    }
    else
    {
        return std::accumulate(text.begin(), text.end(), 0, [&](unsigned sum, char c)
        {
            assert(c!='\n');    // TODO: split lines and compute maximum
            return sum + glutBitmapWidth(font->ptr, c);
        });
    }
}

void Font::Render(int character) const
{
    if (!font)
        return;

    if (font->height == 0)
    {
        glPushMatrix();
        glScaled(height, height, 1);
        glutStrokeCharacter(font->ptr, character);
        glPopMatrix();
    }
    else
    {
        glutBitmapCharacter(font->ptr, character);
    }
}

}

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

static constexpr float strokeFontHeight = 152.38f;  // https://www.opengl.org/resources/libraries/glut/spec3/node78.html

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

Font::Font(std::string_view name, float height) :
    font(FindFont(name, height)),
    scale(font ? (font->height ? 1.0 : height ) : 0.0f)
{
    if (!font)
        throw std::invalid_argument(std::format("Unknown font: {} {}", name, height));
}

bool Font::IsStroke() const
{
    return font && font->height == 0;
}

float Font::Height(std::string_view text) const
{
    if (!font)
        return 0;
    unsigned lines = std::count(text.begin(), text.end(), '\n') + 1;
    if (IsStroke())
        return scale * lines;
    else
        return font->height * lines;
}

float Font::Width(std::string_view text) const
{
    float sum = 0;
    if (!font)
        return sum;
    else if (IsStroke())
    {
        return std::accumulate(text.begin(), text.end(), 0.0f, [this](float sum, char c)
        {
            assert(c!='\n');    // TODO: split lines and compute maximum
            float charWidth = glutStrokeWidth(font->ptr, c);
            return sum + charWidth;
        }) * scale / strokeFontHeight;
    }
    else
    {
        return std::accumulate(text.begin(), text.end(), 0, [this](unsigned sum, char c)
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

    if (IsStroke())
    {
        float s = scale / strokeFontHeight;
        glScaled(s, s, 1.0);   // make character height 1.0
        glutStrokeCharacter(font->ptr, character);
        glScaled(1.0 / s, 1.0 / s, 1.0);
    }
    else
    {
        glutBitmapCharacter(font->ptr, character);
    }
}

}

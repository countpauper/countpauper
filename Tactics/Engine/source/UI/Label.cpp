#include "UI/Label.h"
#include "Rendering/Text.h"
#include "Utility/Assert.h"
#include <GL/gl.h>

namespace Engine
{

Label::Label(std::string_view name, std::string_view text, float lines) :
    Control(name),
    text(text),
    font("stroke roman", 1.0f / lines)
{
}

void Label::SetText(std::string_view newText)
{
    text=newText;
}

void Label::AddText(std::string_view newText)
{
    text += newText;
}

void Label::RenderBackground() const
{
    if (background)
    {
        background.Render();
        glBegin(GL_QUADS);
            glVertex2f(0.0f, 0.0f);
            glVertex2f(0.0f, 1.0f);
            glVertex2f(1.0f, 1.0f);
            glVertex2f(1.0f, 0.0f);
        glEnd();
    }
}

void Label::RenderOutline() const
{
    if (outline)
    {
        outline.Render();
        glLineWidth(1.0f);
        glBegin(GL_LINE_LOOP);
            glVertex2f(0.0f, 0.0f);
            glVertex2f(1.0f, 0.0f);
            glVertex2f(1.0f, 1.0f);
            glVertex2f(0.0f, 1.0f);
        glEnd();
    }
}

void Label::Render() const
{
    if (!shown)
        return;
    RenderBackground();
    RenderOutline();

    if (IsEnabled())
        enabledColor.Render();
    else
        disabledColor.Render();
    glText(text.c_str(), font, horizontal_align, vertical_align);
}

}

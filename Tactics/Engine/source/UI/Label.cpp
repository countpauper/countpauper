#include "UI/Label.h"
#include "Rendering/Text.h"
#include "Utility/Assert.h"
#include <GL/gl.h>

namespace Engine
{

Label::Label(std::string_view name, std::string_view text, float lines) :
    Control(name),
    text(text)
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

void Label::RenderBackground(const Style& style) const
{
    if (style.background)
    {   // NB: sorting wrong, will block off the text
        style.background.Render();
        glBegin(GL_QUADS);
            glVertex2f(0.0f, 0.0f);
            glVertex2f(0.0f, 1.0f);
            glVertex2f(1.0f, 1.0f);
            glVertex2f(1.0f, 0.0f);
        glEnd();
    }
}

void Label::RenderOutline(const Style& style) const
{
    if (style.outline && style.lineWidth>0.0f)
    {
        style.outline.Render();
        glLineWidth(style.lineWidth);
        glBegin(GL_LINE_LOOP);
            glVertex2f(0.0f, 0.0f);
            glVertex2f(1.0f, 0.0f);
            glVertex2f(1.0f, 1.0f);
            glVertex2f(0.0f, 1.0f);
        glEnd();
    }
}

void Label::RenderText(const Style& style) const
{
    glText(text.c_str(), style.font, horizontal_align, vertical_align);
}

void Label::Render() const
{
    if (!shown)
        return;

    RenderBackground(ActiveStyle());
    RenderOutline(ActiveStyle());
    RenderText(ActiveStyle());

}

const Style& Label::ActiveStyle() const
{
    if (IsEnabled())
        return enabledStyle;
    else
        return disabledStyle;
}


}

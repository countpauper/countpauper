#include "UI/Label.h"
#include "Rendering/Text.h"
#include <GL/gl.h>
#include <cassert>

namespace Engine
{

Label::Label(std::string_view name, std::string_view text) :
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

void Label::Render() const
{
    if (!shown)
        return;
    if (IsEnabled())
        enabledColor.Render();
    else
        disabledColor.Render();
    glText(text.c_str(), horizontal_align, vertical_align);
}

}

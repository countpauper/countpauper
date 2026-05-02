#include "UI/Label.h"
#include "Rendering/Text.h"
#include "Utility/Assert.h"
#include <GL/gl.h>

namespace Engine
{

Label::Label(std::string_view name, std::string_view text) :
    Control(name),
    text(text),
    font("helvetica", 12)
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
    glText(text.c_str(), font, horizontal_align, vertical_align);
}

}

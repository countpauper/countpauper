#include "UI/Label.h"
#include "Rendering/Text.h"

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

void Label::Render() const
{
    if (!shown)
        return;
    glText(text.c_str());
}

}

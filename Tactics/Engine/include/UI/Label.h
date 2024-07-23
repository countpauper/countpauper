#pragma once
#include <string>
#include "UI/Control.h"
#include "Rendering/Color.h"
#include "Rendering/Text.h"
namespace Engine
{

class Label : public Control
{
public:
    Label(std::string_view name, std::string_view text="");
    void SetText(std::string_view text);
    void AddText(std::string_view newText);
    // TODO set font, color, use default font, alignment

    void Render() const override;
    Control* Click(Coordinate pos) const override { return nullptr; }

    RGBA enabledColor = RGBA::white;
    RGBA disabledColor = RGBA::white.Translucent(0.5);

    Align vertical_align = Align::top;
    Align horizontal_align = Align::left;
private:
    std::string text;
};

}

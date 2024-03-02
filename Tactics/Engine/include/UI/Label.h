#pragma once
#include <string>
#include "UI/Control.h"
#include "Rendering/Color.h"

namespace Engine
{

class Label : public Control
{
public:
    Label(std::string_view name, std::string_view text="");
    void SetText(std::string_view text);
    // TODO set font, color, use default font
    void Render() const override;
    Control* Click(Coordinate pos) const override { return nullptr; }

    RGBA enabledColor = RGBA::white;
    RGBA disabledColor = RGBA::white.Translucent(0.5);
private:
    std::string text;
};

}

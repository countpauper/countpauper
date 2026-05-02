#pragma once
#include <string>
#include "UI/Control.h"
#include "Rendering/Color.h"
#include "Rendering/Font.h"
#include "Rendering/Text.h"
#include "Rendering/Color.h"

namespace Engine
{

class Label : public Control
{
public:
    Label(std::string_view name, std::string_view text="", float lines = 1.0f);
    void SetText(std::string_view text);
    void AddText(std::string_view newText);

    void Render() const override;
    Control* Click(Coordinate pos) const override { return nullptr; }

    RGBA enabledColor = RGBA::white;
    RGBA disabledColor = RGBA::white.Translucent(0.5);
    Font font;

    Align vertical_align = Align::top;
    Align horizontal_align = Align::left;
    RGBA outline = RGBA::transparent;
    RGBA background = RGBA::transparent;
private:
    void RenderBackground() const;
    void RenderOutline() const;
    std::string text;
};

}

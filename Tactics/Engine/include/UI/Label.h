#pragma once
#include <string>
#include "UI/Control.h"
#include "UI/Style.h"
#include "Rendering/Text.h"

namespace Engine
{

class Label : public Control
{
public:
    Label(std::string_view name, std::string_view text="", float lines = 1.0f);
    void SetText(std::string_view text);
    void AddText(std::string_view newText);

    void Render() const override;
    Control* Click(Coordinate pos) override { return nullptr; }

    Align vertical_align = Align::top;
    Align horizontal_align = Align::left;

    Style enabledStyle{.font=Font("helvetica", 12) };
    Style disabledStyle{.textColor=RGBA::white.Translucent(0.5), .font=enabledStyle.font};
protected:
    virtual const Style& ActiveStyle() const;

    void RenderBackground(const Style& style) const;
    void RenderOutline(const Style& style) const;
    void RenderText(const Style& style) const;
private:
    std::string text;
};

}

#pragma once
#include <string>
#include "UI/Label.h"
#include "UI/Bus.h"

namespace Engine
{

class Button :
    public Label,
    public Passenger
{
public:
    Button(std::string_view name, std::string_view text);
    Control* Click(Coordinate pos) override;

    struct Clicked : public Message
    {
        Clicked(Button& button);
        Button& button;
    };

    void SetHotkey(char ascii);
    void SetHotkey(std::uint8_t code);
    void ResetHotkey();

    Style selectedStyle = {.outline=RGBA::white, .font=enabledStyle.font};
    bool selected;
private:

    const Style& ActiveStyle() const;

    void OnMessage(const Message& message) override;
    std::pair<int, char> hotkey;   // TODO add modifiers but make a better key struct and include it, constructors etc
};

}

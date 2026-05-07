#include "UI/Button.h"
#include "UI/Bus.h"
#include "UI/Application.h"
#include "UI/WindowMessages.h"

namespace Engine
{

Button::Button(std::string_view name, std::string_view text) :
    Label(name, text, 1),
    hotkey{0,0},
    selected(false)
{
    enabled = true;
    enabledStyle.outline = Engine::RGBA(128, 128, 128);
    disabledStyle.outline = Engine::RGBA(64, 64, 64);
}

Control* Button::Click(Coordinate pos)
{
    if (IsEnabled())
        Application::Get().bus.Post(Clicked(*this));
    return this;
}

void Button::SetHotkey(char ascii)
{
    hotkey = std::make_pair(0, ascii);
    Engine::Application::Get().bus.Subscribe(*this, { Engine::MessageType<Engine::KeyPressed>() });
}

void Button::SetHotkey(std::uint8_t code)
{
    hotkey = std::make_pair(code, 0);
    Engine::Application::Get().bus.Subscribe(*this, {  Engine::MessageType<Engine::KeyPressed>() });
}

void Button::ResetHotkey()
{
    Engine::Application::Get().bus.Unsubscribe(*this, {  Engine::MessageType<Engine::KeyPressed>() });
}



const Style& Button::ActiveStyle() const
{
    if (selected)
        return selectedStyle;
    else
        return Label::ActiveStyle();
}


void Button::OnMessage(const Message& message)
{
    if (auto key = message.Cast<Engine::KeyPressed>())
    {
        if (hotkey == std::make_pair(key->code, key->ascii) &&
            IsEnabled())
        {
            Application::Get().bus.Post(Clicked(*this));
        }
    }
}


Button::Clicked::Clicked(Button& button) :
    button(button)
{
}


}

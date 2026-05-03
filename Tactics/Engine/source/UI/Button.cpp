#include "UI/Button.h"
#include "UI/Bus.h"
#include "UI/Application.h"
#include "UI/WindowMessages.h"

namespace Engine
{

Button::Button(std::string_view name, std::string_view text) :
    Label(name, text, 1)
{
    enabled = true;

    Engine::Application::Get().bus.Subscribe(*this,
    {
        Engine::MessageType<Engine::KeyPressed>()
    });

}

Control* Button::Click(Coordinate pos)
{
    if (!IsEnabled())
        return nullptr;

    Application::Get().bus.Post(Clicked(*this));
    return this;
}


void Button::OnMessage(const Message& message)
{
    // TODO Hotkey
}


Button::Clicked::Clicked(Button& button) :
    button(button)
{
}


}

#include "UI/Button.h"
#include "UI/Bus.h"
#include "UI/Application.h"

namespace Engine
{

struct ButtonClicked : public Message
{
    ButtonClicked(const Button& button) :
        button(button)
    {
    }
    const Button& button;
};

Button::Button(std::string_view name, std::string_view text) :
    Label(name, text, 1)
{
    enabled = true;
}

Control* Button::Click(Coordinate pos)
{
    if (!IsEnabled())
        return nullptr;

    Application::Get().bus.Post(ButtonClicked(*this));
    return this;
}

}

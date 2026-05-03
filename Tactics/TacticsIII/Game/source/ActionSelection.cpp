#include "UI/ActionSelection.h"
#include "UI/GameMessages.h"
#include "UI/Button.h"
#include "UI/Application.h"
#include "UI/Avatar.h"
#include "UI/Window.h"
#include "UI/Logging.h"
#include <ranges> 

namespace Game::UI
{

ActionSelection::ActionSelection()
    : selectedButton(nullptr)
{   
    Engine::Application::Get().bus.Subscribe(*this,
    {
        Engine::MessageType<UI::Selected>(),
        Engine::MessageType<Engine::Button::Clicked>()
    });
    HideActions();
}

void ActionSelection::OnMessage(const Engine::Message& message)
{
    if (auto selected = message.Cast<UI::Selected>())
    {
        if (selected->avatar)
            ShowActions(*selected->avatar);
        else
            HideActions();
    }
    else if (auto clicked = message.Cast<Engine::Button::Clicked>())
    {
        if (selectedButton)
        {
            selectedButton->outline = Engine::RGBA(128, 128, 128);
        }
        auto& button = clicked->button;
        if (selectedButton == &button)
        {
            Engine::Logging::Log<Engine::Logging::Info, Engine::Logging::Info>("Deselected (%s)", button.Name().c_str());
            selectedButton = nullptr;
        }
        else 
        {
            selectedButton = &button;   
            Engine::Logging::Log<Engine::Logging::Info, Engine::Logging::Info>("Selected (%s)", button.Name().c_str());
            selectedButton->outline = Engine::RGBA::white;
        }
    }
}

std::vector<std::reference_wrapper<Engine::Button>> GetActionButtons()
{
    std::vector<std::reference_wrapper<Engine::Button>> buttons;
    for(int i=0; i<10; ++i)
    {
        // TODO: Could be faster to find action bar(s) and add all their button children
        auto* button = Engine::Window::CurrentWindow()->GetHUD().Find<Engine::Button>(std::format("action{}",i));
        assert(button);
        buttons.emplace_back(*button);
    }
    return buttons;
}

void ActionSelection::HideActions()
{
    for(auto button : GetActionButtons())
    {
        button.get().Hide();
    }
}

void ActionSelection::ShowActions(const class Avatar& avatar)
{
    std::array<std::string_view, 10> actionName = {
        "up",
        "down",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "end"
    };
    for(auto [button, name] : std::views::zip(GetActionButtons(), actionName))
    {
        button.get().Show();
        button.get().SetText(name);
    }
}

}
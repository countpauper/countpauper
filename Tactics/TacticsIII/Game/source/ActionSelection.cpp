#include "UI/ActionSelection.h"
#include "UI/GameMessages.h"
#include "UI/Button.h"
#include "UI/Application.h"
#include "UI/Avatar.h"
#include "UI/Window.h"
#include "UI/Logging.h"
#include "UI/WindowMessages.h"
#include <ranges> 

namespace Game::UI
{


std::vector<std::reference_wrapper<Engine::Button>> FindActionButtons()
{
    std::vector<std::reference_wrapper<Engine::Button>> buttons;
    for(int i=0; i<10; ++i)
    {
        // TODO: Could be faster to find action bar(s) and add all their button children
        auto* button = Engine::Window::CurrentWindow()->GetHUD().Find<Engine::Button>(std::format("action{}",i));
        assert(button);
        button->SetHotkey(static_cast<char>('0' + (i+1)%10));        
        buttons.emplace_back(*button);
    }
    return buttons;
}


ActionSelection::ActionSelection()
    : selectedButton(nullptr)
    , actionButtons(FindActionButtons())
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

            if (auto it = actionFactories.find(&button); it != actionFactories.end())
            {
                auto plan = it->second();
            }
        }
        Engine::Application::Get().bus.Post(Engine::Redraw());
    }
}

void ActionSelection::HideActions()
{
    for(auto button : actionButtons)
    {
        button.get().Hide();
    }
    actionFactories.clear();
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
    int hotkey = 1;
    for(auto [button, name] : std::views::zip(actionButtons, actionName))
    {
        button.get().Show();
        button.get().SetText(name);

        if (!name.empty())
        {
            auto& btn = button.get();
            if (name == "up")
            {
                actionFactories[&btn] = [&avatar]() { return Plan(); };
            }
            else if (name == "down")
            {
                actionFactories[&btn] = [&avatar]() { return Plan(); };
            }
            else if (name == "end")
            {
                actionFactories[&btn] = [&avatar]() { return Plan(); };
            }
        }

        hotkey++;
    }
}

}
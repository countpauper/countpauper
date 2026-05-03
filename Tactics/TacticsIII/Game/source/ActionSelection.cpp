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


ActionSelection::SelectableActions ActionSelection::FindButtons()
{
    SelectableActions buttons;
    for(int i=0; i<10; ++i)
    {
        // TODO: Could be faster to find action bar(s) and add all their button children
        auto* button = Engine::Window::CurrentWindow()->GetHUD().Find<Engine::Button>(std::format("action{}",i));
        assert(button);
        button->SetHotkey(static_cast<char>('0' + (i+1)%10));        
        buttons.emplace(button, ActionFactory());
    }
    return buttons;
}


ActionSelection::ActionSelection()
    : selectedButton(nullptr)
    , actionButtons(FindButtons())
{   
    Engine::Application::Get().bus.Subscribe(*this,
    {
        Engine::MessageType<UI::Selected>(),
        Engine::MessageType<Engine::Button::Clicked>()
    });

    StyleButtons();
}

void ActionSelection::OnMessage(const Engine::Message& message)
{
    if (auto selected = message.Cast<UI::Selected>())
    {
        if (selected->avatar)
            PopulateButtons(*selected->avatar);
        else
            DepopulateButtons();
        StyleButtons();
    }
    else if (auto clicked = message.Cast<Engine::Button::Clicked>())
    {
        auto it = actionButtons.find(&clicked->button);
        if (it == actionButtons.end())
            return;
        
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
        Engine::Application::Get().bus.Post(Engine::Redraw());
    }
}

void ActionSelection::DepopulateButtons()
{
    for(auto& button : actionButtons)
    {
        button.first->Hide();
        button.second = ActionFactory();
    }
}

void ActionSelection::PopulateButtons(const class Avatar& avatar)
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
    for(auto [buttonItem, name] : std::views::zip(actionButtons, actionName))
    {
        auto& button = *buttonItem.first;
        button.SetText(name);
        if (!name.empty())
        {
            if (name == "up")
            {
                buttonItem.second = [&avatar]() { return Plan(); };
            }
            else if (name == "down")
            {
                buttonItem.second = [&avatar]() { return Plan(); };
            }
            else if (name == "end")
            {
                buttonItem.second = [&avatar]() { return Plan(); };
            }
            else 
            {
                buttonItem.second = ActionFactory();
            }
        }
    }
}


void ActionSelection::StyleButtons()
{
    for(auto& button : actionButtons)
    {
        if (button.second)
            button.first->Enable();
        else 
            button.first->Disable();
    }
}

}
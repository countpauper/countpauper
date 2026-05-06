#include "Game/UpDown.h"
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
        buttons.emplace(button, ButtonAction());
    }
    return buttons;
}


ActionSelection::ActionSelection(World& world)
    : world(world)
    , selectedButton(nullptr)
    , selectedActor(nullptr)
    , actionButtons(FindButtons())
{   
    Engine::Application::Get().bus.Subscribe(*this,
    {
        Engine::MessageType<UI::Selected>(),
        Engine::MessageType<Engine::Button::Clicked>(),
        Engine::MessageType<PlanFactoryIF::Complete>()
    });

    StyleButtons();
    SelectDefault();
}

void ActionSelection::Select(Engine::Button& button)
{
    selectedButton = &button;
    Engine::Logging::Log<Engine::Logging::Info, Engine::Logging::Info>("Selected (%s)", button.Name().c_str());
    selectedButton->outline = Engine::RGBA::white;
    auto it = actionButtons.find(selectedButton);
    it->second->Activate();
    defaultFactory.Deactivate();
}

void ActionSelection::Deselect()
{
    if (selectedButton)
    {
        Engine::Logging::Log<Engine::Logging::Info, Engine::Logging::Info>("Deselected (%s)", selectedButton->Name().c_str());
        auto it = actionButtons.find(selectedButton);
        it->second->Deactivate();
        selectedButton->outline = Engine::RGBA(128, 128, 128);
        selectedButton = nullptr;
    }
}

void ActionSelection::SelectDefault()
{
    Deselect();
    defaultFactory.Activate();
}

void ActionSelection::OnMessage(const Engine::Message& message)
{
    if (auto selected = message.Cast<UI::Selected>())
    {
        DepopulateButtons();
        if (selected->avatar)
            PopulateButtons(*selected->avatar);
        StyleButtons();
        selectedActor = selected->avatar;
    }
    else if (auto clicked = message.Cast<Engine::Button::Clicked>())
    {
        auto it = actionButtons.find(&clicked->button);
        if (it == actionButtons.end())
            return;
        
        auto& button = clicked->button;
        if (selectedButton == &button)
        {
            SelectDefault();
        }
        else
        {
            Deselect();
            Select(button);
        }
        Engine::Application::Get().bus.Post(Engine::Redraw());
    }
    else if (auto complete = message.Cast<PlanFactoryIF::Complete>())
    {
        if (selectedButton && 
            actionButtons.find(selectedButton)->second.get() == &complete->factory)
        {
            SelectDefault();
        }
    }
}


void ActionSelection::DepopulateButtons()
{
    Deselect();
    for(auto& button : actionButtons)
    {
        button.first->Hide();
        button.second.reset();
    }
}

void ActionSelection::PopulateButtons(const class Avatar& avatar)
{
    // TODO get the name from the action factory, ie from the action statically? Or the other way around (not from the model but from the view )
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
        button.Show();
        if (!name.empty())
        {
            if (name == "up")
            {
                buttonItem.second = std::move(std::make_unique<PlanFactoryAction<Up>>());
            }
            else if (name == "down")
            {
                buttonItem.second = std::move(std::make_unique<PlanFactoryAction<Down>>());
            }
            else if (name == "end")
            {
                // TODO some other lambda with world and actor? 
            }
            else 
            {
                buttonItem.second.reset();
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
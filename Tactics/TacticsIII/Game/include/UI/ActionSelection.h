#pragma once

#include "UI/Bus.h"
#include "UI/Button.h"
#include "Game/Plan.h"
#include <map>
#include <functional>

namespace Game::UI
{
class ActionSelection : public Engine::Passenger
{
public:
    ActionSelection();
private:
    void OnMessage(const Engine::Message& message) override;

    using ActionFactory = std::function<Plan()>;
    using SelectableActions = std::map<Engine::Button*, std::function<Plan()>>;
    SelectableActions FindButtons();

    void DepopulateButtons();
    void PopulateButtons(const class Avatar& avatar);
    void StyleButtons();
private:
    
    Engine::Button* selectedButton;
    SelectableActions actionButtons;
};

}
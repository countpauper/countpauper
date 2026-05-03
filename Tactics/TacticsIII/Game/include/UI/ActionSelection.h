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
    void HideActions();
    void ShowActions(const class Avatar& avatar);
private:
    Engine::Button* selectedButton;
    std::map<Engine::Button*, std::function<Plan()>> actionFactories;
    std::vector<std::reference_wrapper<Engine::Button>> actionButtons;
};

}
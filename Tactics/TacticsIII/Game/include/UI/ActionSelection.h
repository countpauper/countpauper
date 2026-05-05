#pragma once

#include "UI/Bus.h"
#include "UI/Button.h"
#include "Game/Plan.h"
#include "Game/ActionFactory.h"
#include <map>
#include <functional>

namespace Game::UI
{
class ActionSelection : public Engine::Passenger
{
public:
    ActionSelection(World& world);
private:
    void OnMessage(const Engine::Message& message) override;

    using ButtonAction = std::unique_ptr<ActionFactoryIF>;
    using SelectableActions = std::map<Engine::Button*, ButtonAction>;
    SelectableActions FindButtons();

    void DepopulateButtons();
    void PopulateButtons(const class Avatar& avatar);
    void StyleButtons();
private:
    void Activate(ActionFactoryIF& factory);
    World& world;
    Engine::Button* selectedButton;
    class Actor* selectedActor;
    SelectableActions actionButtons;
};

}
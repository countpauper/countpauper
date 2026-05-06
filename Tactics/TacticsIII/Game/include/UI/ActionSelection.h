#pragma once

#include "UI/Bus.h"
#include "UI/Button.h"
#include "Game/Plan.h"
#include "Game/PlanFactory.h"
#include "Game/Move.h"
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

    using ButtonAction = std::unique_ptr<PlanFactoryIF>;
    using SelectableActions = std::map<Engine::Button*, ButtonAction>;
    SelectableActions FindButtons();

    void DepopulateButtons();
    void PopulateButtons(const class Avatar& avatar);
    void StyleButtons();
private:
    void Select(Engine::Button& button);
    void Deselect();
    void SelectDefault();
    
    World& world;
    PlanFactoryAction<Move> moveFactory; 
    // TODO attackFactory, activate by default;
    Engine::Button* selectedButton;
    class Actor* selectedActor;
    SelectableActions actionButtons;
};

}
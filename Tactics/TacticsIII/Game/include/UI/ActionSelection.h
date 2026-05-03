#pragma once 

#include "UI/Bus.h"
#include "UI/Button.h"

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
    
};

}
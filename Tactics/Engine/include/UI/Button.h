#pragma once
#include <string>
#include "UI/Label.h"
#include "UI/Bus.h"

namespace Engine
{



class Button :
    public Label,
    public Passenger
{
public:
    Button(std::string_view name, std::string_view text);
    Control* Click(Coordinate pos) override;

    struct Clicked : public Message
    {
        Clicked(Button& button);
        Button& button;
    };
private:
    void OnMessage(const Message& message) override;
};

}

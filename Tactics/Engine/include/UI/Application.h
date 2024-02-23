#pragma once

#include "UI/Bus.h"

namespace Engine
{

class Application : public Passenger
{
public:
    Application(int argc=0, char** argv=nullptr);
    void Run();
    Bus bus;
    static Application& Get();

    void OnMessage(const Message& message);
};

}

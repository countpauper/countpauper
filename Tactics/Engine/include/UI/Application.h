#pragma once

#include "Utility/Singleton.h"
#include "UI/Bus.h"

namespace Engine
{

class Application
{
public:
        Application(int argc=0, char** argv=nullptr);

        void Run();
};

}

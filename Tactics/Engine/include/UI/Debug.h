#pragma once

#include "Utility/Timer.h"
#include "UI/Logging.h"
#include <string>

namespace Engine::Logging
{
	class Timer final
	{
	public:
        Timer(std::string_view desc);
        ~Timer();
	private:
        std::string description;
        Engine::Timer timer;
    };
}

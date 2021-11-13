#pragma once

#include "Timer.h"
#include <string>

namespace Engine::Debug
{
	void Log(std::string_view txt);
	void Log(std::wstring_view txt);

	class Timer final
	{
	public:
        Timer(std::wstring_view desc);
        ~Timer();
	private:
        std::wstring description;
        Engine::Timer timer;
    };
}

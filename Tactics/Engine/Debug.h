#pragma once

#include "Timer.h"
#include <string>
#include <sstream>

namespace Engine::Debug
{
    void Log(std::string_view txt);
    class LogStream
    {
    public:
        LogStream() = default;
        LogStream(const LogStream&) = delete;
        LogStream(LogStream&& o);
        ~LogStream();
        template<typename T>
        LogStream& operator<<(const T& v)
        {
            s << v;
            return *this;
        }
    private:
        std::ostringstream s;
    };

    LogStream Log(); 

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

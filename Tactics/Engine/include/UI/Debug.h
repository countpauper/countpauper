#pragma once

#include "Utility/Timer.h"
#include <string>
#include <sstream>

namespace Engine::Debug
{
    template<typename... T>
    void Log(const char* s, T... parameters)
    {
        static char lineBuffer[1024];
        int length = snprintf(lineBuffer,sizeof(lineBuffer), s, std::forward<T>(parameters)...);
        Log(lineBuffer);
    }

    template<>
    void Log(const char* s);

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

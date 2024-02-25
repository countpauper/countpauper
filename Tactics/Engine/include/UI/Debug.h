#pragma once

#include "Utility/Timer.h"
#include <string>
#include <sstream>

namespace Engine::Debug
{
    enum LogLevel
    {
        Trace=0,
        Debug=1,
        Info=2,
        Warning=3,
        Error=4,
        Fatal=5
    };


    template<bool ET, typename... Params>
    struct Logit
    {
        Logit(const char*, Params...)
        {
        }

    };

    template<>
    struct Logit<true>
    {
        Logit(const char* s);
    };

    //template struct Logit<true>;

    template<typename... Params>
    struct Logit<true, Params...>
    {
        Logit(const char* s, Params... parameters)
        {
            static char lineBuffer[1024];
            snprintf(lineBuffer,sizeof(lineBuffer), s, std::forward<Params>(parameters)...);
            Logit<true> log(lineBuffer);
        }
    };

/*
    template<bool ET, typename... T>
    struct LogParams;

    template<typename... T>
    struct LogParams<true, T...>
    {
        LogParams(const char* s, T... parameters)
        {
            static char lineBuffer[1024];
            int length = snprintf(lineBuffer,sizeof(lineBuffer), s, std::forward<T>(parameters)...);
            Logit<true> log(lineBuffer);
        }
    };

    template<typename... T>
    struct LogParams<false, T...>
    {
        LogParams(const char* s, T... parameters) {}
    };
*/


/*    template<bool ET, typename... T>
    void Log(const char* s, T... parameters)
    {
        //LogParams<true, int> log(s, std::forward<T>(parameters)...);
    }
*/

    template<bool ET, typename... Params>
    void Log(const char* s, Params... params)
    {
        Logit<ET, Params...> log(s, std::forward<Params>(params)...);
    }

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

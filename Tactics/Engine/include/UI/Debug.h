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


    template<typename ET>
    struct Logit
    {
        Logit(const char* s)
        {
        }

    };
    template<>
    struct Logit<std::true_type>
    {
        Logit(const char* s)
        {
            printf(s);  // TODO: return timer use an internal logbuffer to sprintf to
        }
    };

    template<bool ET, typename Enable, typename... T>
    struct LogParams;

    template<bool ET, typename... T>
    struct LogParams<ET, typename std::enable_if_t<ET>, T...>
    {
        LogParams(const char* s, T... parameters)
        {
            static char lineBuffer[1024];
            int length = snprintf(lineBuffer,sizeof(lineBuffer), s, std::forward<T>(parameters)...);
            Logit<std::true_type> log(lineBuffer);
        }
    };

    template<bool ET, typename... T>
    struct LogParams<ET, typename std::enable_if_t<!ET>, T...>
    {
        LogParams(const char* s, T... parameters) {}
    };


    template<bool ET, typename...T>
    struct LogParams : public detail::A_impl<T, void, Args...> {};

    template<bool ET, typename... T>
    void Log(const char* s, T... parameters)
    {
        //LogParams<true, int> log(s, std::forward<T>(parameters)...);
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

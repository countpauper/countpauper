#pragma once

#include <string>
#include <sstream>

namespace Engine::Logging
{
    enum Level
    {
        Off = 0,
        Fatal,
        Error,
        Warning,
        Info,
        Debug,
        Trace,
        Full=Trace
    };

    namespace Internal
    {
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

        template<Level CategoryLevel, Level level>
        constexpr bool LogLevelOn()
        {
            return static_cast<int>(CategoryLevel) >= static_cast<int>(level);
        }
    }

    template<bool On, typename... Params>
    void Log(const char* s, Params... params)
    {
        Internal::Logit<On, Params...> log(s, std::forward<Params>(params)...);
    }
    template<Level CatLvl, Level LogLvl, typename... Params>
    void Log(const char* s, Params... params)
    {
        Internal::Logit<Internal::LogLevelOn<CatLvl, LogLvl>(), Params...> log(s, std::forward<Params>(params)...);
    }

    // TODO add log levels or just remove it 
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
}


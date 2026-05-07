#pragma once

#include <sstream>
#include <utility>
#include <format>

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
            Logit(std::format_string<Params...>, Params&&...)
            {
            }
        };

        template<>
        struct Logit<true>
        {
            Logit(std::string_view s);
            Logit(std::format_string<> fmt):
                Logit(std::string_view(std::format(fmt)))
            {
            }
        };

        template<typename... Params>
        struct Logit<true, Params...>
        {
            Logit(std::string_view s)
            {
                Logit<true> log(s);
            }
            Logit(std::format_string<Params...> fmt, Params&&... parameters) :
                Logit(std::format(fmt, std::forward<Params>(parameters)...))
            {
            }
        };

        template<Level CategoryLevel, Level level>
        constexpr bool LogLevelOn()
        {
            return static_cast<int>(CategoryLevel) >= static_cast<int>(level);
        }
    }

    template<bool On, typename... Params>
    void Log(std::format_string<Params...> fmt, Params&&... params)
    {
        Internal::Logit<On, Params...> log(fmt, std::forward<Params>(params)...);
    }


    template<Level CatLvl, Level LogLvl, typename... Params>
    void Log(std::format_string<Params...> fmt, Params&&... params)
    {
        Internal::Logit<Internal::LogLevelOn<CatLvl, LogLvl>(), Params...> log(fmt, std::forward<Params>(params)...);
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


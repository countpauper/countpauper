#include <iostream>
#include <cstdio>
#include "Utility/Timer.h"
#include "UI/Logging.h"

namespace Engine::Logging
{

    namespace Internal
    {
        static Engine::Timer startup;
        Logit<true>::Logit(std::string_view s)
        {
            printf("[%.3f] %.*s\n", startup.Seconds(), static_cast<int>(s.size()), s.data());
        }

    }

LogStream Log()
{
    return LogStream();
}

LogStream::LogStream(LogStream&& o)
{
    s << o.s.str();
}

LogStream::~LogStream()
{
    Internal::Logit<true>(std::string_view(s.str()));
}

}

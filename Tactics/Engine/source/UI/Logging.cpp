#include <iostream>
#include <cstdio>
#include "Utility/Timer.h"
#include "UI/Logging.h"

namespace Engine::Logging
{

    namespace Internal
    {
        static Engine::Timer startup;
        Logit<true>::Logit(const char* s)
        {
            printf("[%.3f] %s\n", startup.Seconds(), s);
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
    Log<true>(s.str().c_str());
}

}

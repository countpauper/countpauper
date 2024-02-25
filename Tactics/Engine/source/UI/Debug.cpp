#if defined(WIN32)
 #include <debugapi.h>
#endif
#include <iostream>
#include <cstdio>
#include "UI/Debug.h"

namespace Engine::Debug
{

static Engine::Timer startup;


Logit<true>::Logit(const char* s)
{
    printf("%.3f %s", startup.Seconds(), s);
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

Timer::Timer(std::string_view desc) :
    description(desc.data())
{
}

Timer::~Timer()
{
    auto t = timer.Seconds();
    Log<true>("%.*s: %.3fms", description.size(), description.data(), t * 1000.0);
}

}

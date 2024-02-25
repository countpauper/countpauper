#if defined(WIN32)
 #include <debugapi.h>
#endif
#include <iostream>
#include <cstdio>
#include "UI/Debug.h"

namespace Engine::Debug
{

static Engine::Timer startup;

/*
template<typename ET>
Logit<std::true_type>::Logit(const char* s)
{
    printf("%.3f %s\n", startup.Seconds(), s);
}
*/

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
    Logit<std::true_type>(s.str().c_str());
}

Timer::Timer(std::string_view desc) :
    description(desc.data())
{
}

Timer::~Timer()
{
    auto t = timer.Seconds();
    LogParams<true> log("%*.s: %dms", description.size(), description.data(), t * 1000.0);
}

}

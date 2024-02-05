#if defined(WIN32) 
 #include <debugapi.h>
#endif 
#include <iostream>
#include "Engine/Debug.h"

namespace Engine::Debug
{

void Log(std::string_view txt)
{
    std::cout << txt.data() << std::endl;
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
    Log(s.str());
}

Timer::Timer(std::string_view desc) :
    description(desc.data())
{
}

Timer::~Timer()
{
    auto t = timer.Seconds();
    Log(description + " : " + std::to_string(t * 1000.0) + "ms");
}

}

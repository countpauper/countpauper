#if defined(WIN32) 
 #include <debugapi.h>
#endif 
#include <iostream>
#include "Engine/Debug.h"

namespace Engine::Debug
{

void Log(std::string_view txt)
{
    #if defined(WIN32) 
    if (GetConsoleWindow())
    {
        std::cout << txt.data() << std::endl;
    }
    else
    {

        OutputDebugStringA(txt.data());
        OutputDebugStringA("\n");
    }
    #else 
        std::cout << txt.data() << std::endl;
    #endif 
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

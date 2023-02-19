#include "stdafx.h"

#include <debugapi.h>
#include <iostream>
#include "Debug.h"

namespace Engine::Debug
{

void Log(std::string_view txt)
{
    if (GetConsoleWindow())
    {
        std::cout << txt.data() << std::endl;
    }
    else
    {
        OutputDebugStringA(txt.data());
        OutputDebugStringA("\n");
    }
}

void Log(std::wstring_view txt)
{
    if (GetConsoleWindow())
    {
        std::wcout << txt.data() << std::endl;
    }
    else
    {
        OutputDebugString(txt.data());
        OutputDebugString(L"\n");
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
    Log(s.str());
}

Timer::Timer(std::wstring_view desc) :
    description(desc.data())
{
}

Timer::~Timer()
{
    auto t = timer.Seconds();
    Log(description + L" : " + std::to_wstring(t * 1000.0) + L"ms");
}

}

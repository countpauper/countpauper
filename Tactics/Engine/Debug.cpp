#include "stdafx.h"

#include <debugapi.h>
#include "Debug.h"

namespace Engine::Debug
{

void Log(std::string_view txt)
{
    OutputDebugStringA(txt.data());
    OutputDebugStringA("\n");
}

void Log(std::wstring_view txt)
{
    OutputDebugString(txt.data());
    OutputDebugString(L"\n");
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

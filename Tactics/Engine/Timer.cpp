#include "stdafx.h"
#include "Timer.h"
#include <windows.h>

namespace Engine
{

Timer::Timer()
{
    LARGE_INTEGER largeInteger;
    QueryPerformanceCounter(&largeInteger);
    start = largeInteger.QuadPart;
}

double Timer::Seconds() const
{
    LARGE_INTEGER now, frequency;
    QueryPerformanceCounter(&now);
    int64_t difference = now.QuadPart - start;
    QueryPerformanceFrequency(&frequency);
    return difference * 1.0 / double(frequency.QuadPart);
}

}   // ::Engine

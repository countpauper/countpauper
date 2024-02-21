#include "Utility/Timer.h"
#include <chrono>

namespace Engine
{

Timer::Timer() :
    start(std::chrono::high_resolution_clock::now())
{
}

double Timer::Seconds() const
{
    const std::chrono::duration<double, std::ratio<1,1>> difference = std::chrono::high_resolution_clock::now() - start;
    return difference.count();
}

}   // ::Engine

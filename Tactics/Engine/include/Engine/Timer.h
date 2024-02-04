#include <chrono>
#pragma once
namespace Engine
{

class Timer
{
public:
    Timer();
    double Seconds() const;
private:
    std::chrono::time_point<std::chrono::high_resolution_clock> start;
};

}
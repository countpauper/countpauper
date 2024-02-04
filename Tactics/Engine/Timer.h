#include <cstdint>
#pragma once
namespace Engine
{

class Timer
{
public:
    Timer();
    double Seconds() const;
private:
    std::int64_t start;
};

}
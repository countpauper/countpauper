#pragma once
namespace Engine
{

class Timer
{
public:
    Timer();
    double Seconds() const;
private:
    int64_t start;
};

}
#pragma once
#include <functional>

namespace Engine
{

class UpdateFrequency
{
public:
    UpdateFrequency(const std::function<void(void)>& update, double frequency=1);
    void Tick(double sec);
private:
    const double m_frequency = 1;
    std::function<void()> m_update;
    double m_remaining = 0;
};

}
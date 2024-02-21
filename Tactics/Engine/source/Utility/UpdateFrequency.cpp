#include "Utility/UpdateFrequency.h"
#include <stdexcept>
#include <cmath>

namespace Engine
{

UpdateFrequency::UpdateFrequency(const std::function<void()>& update, double frequency) :
    m_frequency(frequency),
    m_update(update)
{
    if (m_frequency < 0)
        throw std::invalid_argument("Update frequency most be positive or 0");
}

void UpdateFrequency::Tick(double sec)
{
    double time = m_remaining + sec;
    unsigned updates = static_cast<unsigned>(std::floor(time*m_frequency));
    for (unsigned update = 0; update < updates; ++update)
        m_update();
    if (m_frequency>0)
        m_remaining = std::fmod(time, 1 / m_frequency);
}

}   // ::Engine

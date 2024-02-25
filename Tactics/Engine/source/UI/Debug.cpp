#include "UI/Debug.h"

namespace Engine::Logging
{

Timer::Timer(std::string_view desc) :
    description(desc.data())
{
}

Timer::~Timer()
{
    auto t = timer.Seconds();
    Log<true>("%.*s: %.3fms", description.size(), description.data(), t * 1000.0);
}

}

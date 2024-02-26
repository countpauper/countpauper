#include "Game/Statted.h"
#include "Game/Counter.h"

namespace Game
{

Counter::Counter(std::string_view name, Stat::Id maxStat, Counter::Reset reset, bool resetTo) :
    name(name),
    maxStat(maxStat),
    resetAt(reset),
    resetToMax(resetTo)
{
}
Engine::Range<int> Counter::GetRange(const Statted& stats) const
{
    if (maxStat == Stat::none)
        return Engine::Range<int>(0, std::numeric_limits<int>::max());
    else
        return Engine::Range<int>(0, stats.Get(maxStat).Total());
}


bool Counter::ResetWhen(Reset when) const
{
    if (maxStat == Stat::none)
        return false;
    return static_cast<int>(when) <= static_cast<int>(resetAt);
}

int Counter::ResetTo(const Statted& stats) const
{
    auto range = GetRange(stats);
    if (resetToMax)
    {
        return range.end;
    }
    else
    {
        return range.begin;
    }
}

std::string_view Counter::Name() const
{
    return name;
}

}

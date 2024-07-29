#include "Game/Statted.h"
#include "Game/Stat.h"
#include "Game/Counter.h"
#include "Utility/from_string.h"

namespace Game
{

Counter::Counter(std::string_view name, Stat::Id maxStat, Counter::Reset reset, bool resetTo) :
    name(name),
    maxStat(maxStat),
    resetAt(reset),
    resetToMax(resetTo)
{
}

Counter::Counter(Stat::Id maxStat, const json& j, std::string_view defaultName) :
    name(Engine::get_value_or(j, "name", defaultName)),
    resetToMax(!Engine::get_value_or(j, "reset_to_min", false)),
    maxStat(maxStat)
{
        std::string resetAtStr = Engine::get_value_or<std::string>(j, "reset", "battle");
        resetAt = Engine::from_string(resetAtStr, std::map<std::string_view, Reset>{
            {"never", Reset::never},
            {"battle", Reset::battle},
            {"round", Reset::round},
            {"turn", Reset::turn},
            {"action", Reset::action}
        });
}


unsigned Counter::Available(int used, const Statted& stats) const
{
    if (!maxStat)
        return std::numeric_limits<unsigned>::max();

    int max = stats.Get(maxStat).Total();
    if (used >= max)
    {
        return 0;
    }
    else
    {
        return max - used;
    }
}

bool Counter::ResetWhen(Reset when) const
{
    if (maxStat == Stat::Id::none)
        return false;
    return static_cast<int>(when) <= static_cast<int>(resetAt);
}

std::string_view Counter::Name() const
{
    return name;
}

bool Counter::ForStat(Stat::Id stat) const
{
    return maxStat == stat;
}

}

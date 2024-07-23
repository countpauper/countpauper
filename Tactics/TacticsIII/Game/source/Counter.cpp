#include "Game/Statted.h"
#include "Game/Stat.h"
#include "Game/Counter.h"
#include "Utility/from_string.h"

namespace Game
{

Counter::Counter(std::string_view name, const Stat* maxStat, Counter::Reset reset, bool resetTo) :
    name(name),
    maxStat(maxStat),
    resetAt(reset),
    resetToMax(resetTo)
{
}

Counter::Counter(const Stat& maxStat, const json& j) :
    maxStat(&maxStat)
{
        name = Engine::get_value_or(j, "name", maxStat.Name());
        resetToMax = !Engine::get_value_or(j, "reset_to_min", false);
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

    int max = maxStat->Compute(stats).Total();
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
    if (maxStat == nullptr)
        return false;
    return static_cast<int>(when) <= static_cast<int>(resetAt);
}

std::string_view Counter::Name() const
{
    return name;
}

bool Counter::ForStat(const Stat* stat) const
{
    return maxStat == stat;
}

}

#pragma once
#include "Game/Stat.h"
#include "Utility/Range.h"

namespace Game
{
class Statted;
class Counter
{
public:
    enum class Reset
    {
        never = 0,
        battle = 1,
        round = 2,
        turn = 3,
        action = 4
    };
    Counter(std::string_view name, Stat::Id maxStat=Stat::none, Counter::Reset reset=Reset::never, bool resetToMax=true);
    Engine::Range<int> GetRange(const Statted& creature) const;
    bool ResetWhen(Reset) const;
    int ResetTo(const Statted& creature) const;
    std::string_view Name() const;
private:
    std::string_view name;
    Reset resetAt;
    bool resetToMax;
    Stat::Id maxStat;
};

}

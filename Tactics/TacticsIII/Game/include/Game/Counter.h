#pragma once
#include "Utility/Range.h"
#include "File/Json.h"
#include "Game/Stat.h"

namespace Game
{
class Statted;
class Stat;

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
    Counter(std::string_view name, Stat::Id maxStat=Stat::Id::none, Counter::Reset reset=Reset::never, bool resetToMax=true);
    Counter(Stat::Id maxStat, const json& j, std::string_view defaultName="");
    unsigned Available(int used, const Statted& stats) const;
    bool ResetWhen(Reset) const;
    std::string_view Name() const;
    bool ForStat(Stat::Id stat) const;
private:
    std::string name;
    Reset resetAt;
    bool resetToMax;
    Stat::Id maxStat;
};

}

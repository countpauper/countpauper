#pragma once
#include "Utility/Range.h"
#include "File/Json.h"

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
    Counter(std::string_view name, const Stat* maxStat=nullptr, Counter::Reset reset=Reset::never, bool resetToMax=true);
    Counter(const Stat& maxStat, const json& j);
    unsigned Available(int used, const Statted& stats) const;
    bool ResetWhen(Reset) const;
    std::string_view Name() const;
    bool ForStat(const Stat* stat) const;
private:
    std::string name;
    Reset resetAt;
    bool resetToMax;
    const Stat* maxStat;
};

}

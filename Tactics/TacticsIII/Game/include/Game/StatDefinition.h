#pragma once
#include "Game/Stat.h"
#include "Game/Counter.h"
#include <map>
#include <fstream>
namespace Game
{

class StatDefinition : public std::map<Stat::Id, Stat>
{
public:
    StatDefinition() = default;
    explicit StatDefinition(std::string_view data);
    void Parse(const json& parsed);
    Stat::Id Identify(const Stat* stat) const;

    Counter& Count(Stat::Id id, Counter::Reset reset=Counter::Reset::never, bool resetToMax=true);
    std::vector<const Counter*> GetCounters() const;
    Computation GetPrimaryStat(Stat::Id id) const;
private:
    std::vector<Counter> counters;
};

}

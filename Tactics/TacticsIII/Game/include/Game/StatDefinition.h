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
    void Load(std::istream& data);
    void Load(const char* filename);
    void Parse(std::string_view data);
    Stat::Id Identify(std::string_view name) const;
    Stat::Id Identify(const Stat* stat) const;
    std::string_view Name(Stat::Id) const;

    Counter& Count(Stat::Id id, Counter::Reset reset=Counter::Reset::never, bool resetToMax=true);
    std::vector<const Counter*> GetCounters() const;
    Computation GetPrimaryStat(Stat::Id id) const;
private:
    void Define(json& parsed);
    std::vector<Counter> counters;
    static std::map<Stat::Id, std::string_view> statNames;
};

}

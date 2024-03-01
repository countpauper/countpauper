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
    void Parse(std::string_view data);
    Stat::Id Identify(std::string_view name) const;
    std::vector<const Counter*> GetCounters() const;
private:
    void Define(json& parsed);
    std::vector<Counter> counters;
    static std::map<Stat::Id, std::string_view> statNames;
};

}

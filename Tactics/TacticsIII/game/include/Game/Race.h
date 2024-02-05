#pragma once
#include "Game/Stat.h"
#include <map>
#include <string>

namespace Game
{
class Race
{
public:
        Race(std::string_view name); //, std::initialize_list<std::pair<Stat::Id, int>> boni);
        int Bonus(Stat id) const;
private:
        std::string name;
        std::map<Stat::Id, int> bonus;
};

}

#pragma once
#include "Game/Stat.h"
#include <map>
#include <string>

namespace Game
{
class Race
{
public:
        Race();
private:
        std::string name;
        std::map<Stat::Id, int> bonus;
};

}

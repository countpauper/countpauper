#pragma once
#include "Game/Stat.h"
#include <map>
#include <string>

namespace Game
{
class Spell
{

private:
        std::string name;
        std::map<Stat::Id, int> normal;
        std::map<Stat::Id, int> double;
        // TODO: range, area or is area also possible to double?
};

}

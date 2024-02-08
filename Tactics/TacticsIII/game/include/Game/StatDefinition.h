#pragma once
#include "Game/Stat.h"
#include <map>

namespace Game
{

class StatDefinition : public std::map<Stat::Id, Stat>
{
public:
        void Load(std::string_view data);
        Stat::Id Identify(std::string_view name) const;
private:
        static std::map<Stat::Id, std::string_view> statNames;
};

}

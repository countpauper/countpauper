#pragma once
#include "Game/Stat.h"
#include <map>
#include <string>
#include <initializer_list>

namespace Game
{
class Race
{
public:
        Race(std::string_view name, std::initializer_list<std::pair<const Stat::Id, int>> boni = {});
        std::string_view Name() const;
        int Bonus(Stat::Id id) const;
        const class Item& GetUnarmed() const;
private:
        std::string name;
        std::map<Stat::Id, int> boni;
};

}

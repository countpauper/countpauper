#include "Game/Race.h"

namespace Game
{

Race::Race(std::string_view name, std::initializer_list<std::pair<const Stat::Id, int>> boni) :
        name(name),
        boni(boni)
{
}

std::string_view Race::Name() const
{
        return name;
}

int Race::Bonus(Stat::Id id) const
{
        auto it = boni.find(id);
        if (it == boni.end()) {
                return 0;
        }
        return it->second;
}
}

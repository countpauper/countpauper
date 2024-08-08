#include "Game/Race.h"
#include "Game/Unarmed.h"

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

Computation Race::Bonus(Stat::Id id) const
{
    auto it = boni.find(id);
    if (it == boni.end()) {
            return Computation();
    }
    return Computation(it->second, Name());
}

const Item& Race::GetUnarmed() const
{
    static Unarmed unarmed;
    return unarmed;
}

}

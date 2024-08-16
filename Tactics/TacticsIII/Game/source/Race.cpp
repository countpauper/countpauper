#include "Game/Race.h"
#include "Game/Unarmed.h"

namespace Game
{

Race::Race(std::string_view name, std::initializer_list<std::pair<const Stat::Id, int>> boni) :
    name(name),
    boni(boni)
{
}

Race::Race(const std::string_view name, const nlohmann::json& raceData) :
    Race(name)
{
    boni = Stat::Deserialize(Engine::get_value_or<json>(raceData, "stats", json()));
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

Races::Races(const json& data)
{
    for(auto el: data.items())
    {
        emplace_back(el.key(), el.value());
    }
}

const Race* Races::Find(std::string_view name) const
{
    auto it = std::find_if(begin(), end(), [name](const Race& race)
    {
        return race.Name() == name;
    });
    if (it == end())
        return nullptr;
    return &(*it);
}

const Race& Races::Get(std::string_view name) const
{
    auto racePtr = Find(name);
    if (!racePtr)
        throw std::invalid_argument(std::string(name)+" is not a known race");
    return *racePtr;
}

}

#include "Game/Race.h"
#include "Game/Unarmed.h"

namespace Game
{

Race::Race(std::string_view name, std::initializer_list<std::pair<const Stat::Id, int>> boni) :
    name(name),
    boni(boni),
    size(1)
{
}

Race::Race(const std::string_view name, const nlohmann::json& raceData) :
    Race(name)
{
    boni = Stat::Deserialize(Engine::get_value_or<json>(raceData, "stats", json()));
    auto sizeVector = Engine::get_value_or<std::vector<int>>(raceData, "size", {});
    if (sizeVector.size()>=3)
    {
        size = Engine::Size(sizeVector[0], sizeVector[1], sizeVector[2]);
    }
    else if (sizeVector.size()==0)
    {
        size = Engine::Size(1);
    }
    else
    {
        size = Engine::Size(sizeVector[0]);
    }
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

Engine::Size Race::GetSize() const
{
    return size;
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

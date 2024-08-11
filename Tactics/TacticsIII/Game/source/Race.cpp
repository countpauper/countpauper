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
    const auto& bonusData = Engine::get_value_or<json>(raceData, "stats", json());
    for(auto el: bonusData.items())
    {
        boni[Stat::Identify(el.key())] = el.value().get<int>();
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

std::vector<Race> Race::Parse(const json& data)
{
    std::vector<Race> result;
    for(auto el: data.items())
    {
        result.emplace_back(el.key(), el.value());
    }
    return result;
}

}

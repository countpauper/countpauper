#include "Game/StatDefinition.h"
#include "Utility/String.h"
#include <nlohmann/json.hpp>
#include <iostream>
#include <fstream>

namespace Game
{
using json = nlohmann::json;

void StatDefinition::Load(std::istream& s)
{
    json parsed = json::parse(s);
    Define(parsed);
}

void StatDefinition::Load(const char* filename)
{
    std::ifstream defFile(filename);
    Load(defFile);
}


void StatDefinition::Parse(std::string_view data)
{
    json parsed = json::parse(data);
    Define(parsed);
}

void StatDefinition::Define(json& parsed)
{
    for (json::iterator it = parsed.begin(); it != parsed.end(); ++it)
    {
        const json& stat = it.value();
        auto name = it.key();
        auto id = Identify(name);
        if (id == Stat::none)
        {
            throw std::runtime_error("Unknown stat: " + name);
        }
        emplace(id, Stat(name, stat, *this));

        if (auto counterDefinition = Engine::try_get<json>(stat, "counter"))
        {
                counters.emplace_back(rbegin()->second, *counterDefinition);
        }
    }
}

Stat::Id StatDefinition::Identify(std::string_view name) const
{
    auto lowName = Engine::LowerCase(name);
    for(const auto& stat: statNames)
    {
        if (stat.second == lowName)
        {
                return stat.first;
        }
    }
    return Stat::Id::none;
}

std::vector<const Counter*> StatDefinition::GetCounters() const
{
    std::vector<const Counter*> result;
    result.reserve(counters.size());
    std::transform(counters.begin(), counters.end(), std::back_inserter(result),
        [](const Counter& c)
        {
                return &c;
        });
    return result;
}

std::map<Stat::Id, std::string_view> StatDefinition::statNames =
{
    { Stat::none, "none" },
    { Stat::level, "level" },

    { Stat::str, "strength" },
    { Stat::lift, "lift" },
    { Stat::damage, "damage" },
    { Stat::athletics, "athletics" },

    { Stat::agi, "agility" },
    { Stat::speed, "speed" },
    { Stat::ap, "action" },
    { Stat::dodge, "dodge" },
    { Stat::reflex, "reflex" },

    { Stat::con, "constitution" },
    { Stat::endurance, "endurance" },
    { Stat::health, "health" },
    { Stat::physical, "physical" },

    { Stat::wis, "wisdom" },
    { Stat::knowledge, "knowledge" },
    { Stat::mental, "mental" },
    { Stat::attune, "attunement" },
    { Stat::focus, "focus" },

    { Stat::intel, "intelligence" },
    { Stat::technique, "technique" },
    { Stat::crit, "critical chance" },
    { Stat::magic, "magic" },

    { Stat::defense, "defense" },
    { Stat::offense, "offense" },
    { Stat::hp, "hitpoints" },
    { Stat::jump, "jump"},
    { Stat::block, "block" },

    { Stat::reach, "reach" },
    { Stat::range, "range" },
    { Stat::hands, "hands" },
    { Stat::hold, "hold" },
    { Stat::weight, "weight" },
    { Stat::wield, "wield" },
    { Stat::enchantment, "enchantment" },
    { Stat::price, "price" },
    { Stat::rarity, "rarity" },

    { Stat::blunt_resist, "blunt resistance" },
    { Stat::sharp_resist, "sharp resistance" },
    { Stat::fire_resist, "fire resistance" },
    { Stat::cold_resist, "cold resistance" },
    { Stat::lightning_resist, "lightning resistance" },
    { Stat::poison_resist, "poison resistance" }
};

}

#include "Game/StatDefinition.h"
#include "Engine/Utils.h"
#include <nlohmann/json.hpp>
#include <iostream>

namespace Game
{
using json = nlohmann::json;

void StatDefinition::Load(std::string_view data)
{
        json parsed = json::parse(data);
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
        { Stat::ap, "action points" },
        { Stat::dodge, "dodge" },
        { Stat::reflex, "reflex" },

        { Stat::con, "constitution" },
        { Stat::endurance, "endurance" },
        { Stat::hp, "hitpoints" },
        { Stat::physical, "physical" },

        { Stat::wis, "wisdom" },
        { Stat::knowledge, "knowledge" },
        { Stat::mental, "mental" },
        { Stat::attune, "attunement" },

        { Stat::intel, "intelligence" },
        { Stat::skill, "skill" },
        { Stat::crit, "critical chance" },
        { Stat::magic, "magic" },

        { Stat::reach, "reach" },
        { Stat::range, "range" },
        { Stat::defense, "defense" },
        { Stat::offense, "offense" },
        { Stat::hands, "hands" },
        { Stat::weight, "weight" },
        { Stat::capacity, "capacity" },

        { Stat::blunt_resist, "blunt resistance" },
        { Stat::sharp_resist, "resist resistance" },
        { Stat::fire_resist, "fire resistance" },
        { Stat::cold_resist, "cold resistance" },
        { Stat::lightning_resist, "lightning resistance" },
        { Stat::poison_resist, "poison resistance" }
};

}

#include "Game/StatDefinition.h"
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
                auto id = static_cast<Stat::Id>(stat["Id"].template get<int>());
                emplace(id, it.key());
        }
        for(json::iterator it = parsed.begin(); it !=parsed.end(); ++it)
        {
                auto id = Find(it.key());
                Stat& stat = at(id);
                stat.Load(it.value(), *this);
        }
}

Stat::Id StatDefinition::Find(std::string_view name) const
{
        for(const auto& stat: *this)
        {
                if (stat.second.Name() == name)
                {
                        return stat.first;
                }
        }
        return Stat::Id::none;
}

}

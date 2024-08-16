#include "Game/Stat.h"
#include "Game/Statted.h"
#include "Game/StatDefinition.h"
#include "Game/Counter.h"
#include "Utility/String.h"
#include <nlohmann/json.hpp>
#include <optional>

namespace Game
{

Stat::Stat() = default;

Stat::Stat(std::string_view name, std::string_view description, const Restrictions& restricted)
    : name(name)
    , description(description)
    , resricted(restricted)
{
}

Stat::Stat(std::string_view name, std::string_view description, Id dependency, std::span<int> table, int multiplier, const Restrictions& restricted)
    : name(name)
    , description(description)
    , dependency(dependency)
    , table(table.begin(), table.end())
    , multiplier(multiplier)
    , resricted(restricted)
{
}

Stat::Stat(std::string_view name, std::string_view description, Stat::Id dependency,
    Operator op, Stat::Id operand, const Restrictions& restricted)
    : name(name)
    , description(description)
    , dependency(dependency)
    , op(op)
    , operand(operand)
    , resricted(restricted)
{
}

Stat::Stat(std::string_view name, const json& j, const StatDefinition& dependencies) :
        name(name)
{       // TODO: this is pretty close to ns::from_json

    // TODO: this can be refactored
    description = Engine::get_value_or(j, "description", std::string());
    if (auto dependName = Engine::try_get<std::string>(j, "depends"))
    {
        dependency = Stat::Identify(*dependName);
    } else {
        dependency = Stat::none;
    }
    table = Engine::get_value_or(j, "table", std::vector<int>());
    if (Engine::get_value_or(j, "integer", false ))
    {
        limit = Engine::Range<int>::max();
    }
    else
    {
        limit = Engine::Range<int>(0, std::numeric_limits<int>::max());
    }
    limit.begin = Engine::get_value_or(j, "min", limit.begin);
    limit.end = Engine::get_value_or(j, "max", limit.end);

    op = Operator::nop;
    operand = Stat::none;
    if (auto addOperand = Engine::try_get<std::string>(j, "+"))
    {
        op = Operator::add;
        operand = Stat::Identify(*addOperand);
    }
    if (auto mulOperand = Engine::try_get<json>(j, "*"))
    {
        if (mulOperand->is_number_integer())
        {
            // would just add it to the table then?
            mulOperand->get_to(multiplier);
        }
        else if (mulOperand->is_string())
        {
            assert(operand == Stat::none);  // already used by add?
            operand = Stat::Identify(mulOperand->template get<std::string>());
            multiplier = 1;
            op = Operator::multiply;
        }
    }
    resricted = Restrictions::Parse(j, "if");
}

Stat::~Stat() = default;

Computation Stat::Compute(const Statted& object, const class Boni* extraBoni, const Restrictions& restricted) const
{
    Computation result(limit);
    if (dependency)
    {
        result += object.Get(dependency, extraBoni, resricted);
    }
    if (!table.empty())
    {
        // TODO table[0] is base, which doesnt matter much except for Computation
        // especially dodge 50% is just base and the rest 5% is 50+25[agi]
        // HD might be 5+2[con]
        auto index = result.Total();
        if (index >= table.size())
        {
            result = Computation(table.back(), result.Description());
        }
        else if (index <= 0)
        {
            result = Computation(table.front(), result.Description());
        }
        else
        {
            result = Computation(table.at(index), result.Description());
        }
    }
    if (op != Operator::nop && operand !=Stat::none)
    {
        auto right = object.Get(operand, extraBoni, resricted);
        switch(op)
        {
            case Operator::multiply:
                result *= right;
                break;
            case Operator::add:
                result += right;
                break;
            default:
                    break;
        }
    }
    if (multiplier != 1)
            result *= Computation(multiplier, name);

/*
    if (result.empty())
    {
            if (auto mockId = object.Definition().Identify(Name()))
            {
                    // NB this could cause infinite recursion, unless object is mocked or its a primary stat

                    auto mockStat = object.Get(mockId, restricted);
                    if (!mockStat.empty())
                            result = mockStat;
            }
    }
*/
    return result;
}

std::string_view Stat::Name() const
{
    return name;
}

std::string_view Stat::Description() const
{
    return description;
}

Engine::Range<int> Stat::Limit() const
{
    return limit;
}



bool Stat::IsPrimary() const
{
    return dependency == Stat::none;

}

Stat::Id Stat::TryIdentify(std::string_view name)
{
    auto lowName = Engine::LowerCase(name);
    for(const auto& stat: statNames)
    {
        if (stat.second == lowName)
        {
                return stat.first;
        }
    }
    return Stat::none;
}

Stat::Id Stat::Identify(std::string_view name)
{
    if (name.empty() || name=="none")
        return Stat::none;
    auto stat = TryIdentify(name);
    if (stat==Stat::none)
        throw std::invalid_argument(std::string("Unknown stat name:") + std::string(name));
    return stat;
}

std::string_view Stat::Name(Stat::Id id)
{
    return statNames.at(id);
}

std::map<Stat::Id, int> Stat::Deserialize(const json& statData)
{
    std::map<Stat::Id, int> result;
    for(auto el: statData.items())
    {
        auto stat = TryIdentify(el.key());
        if (stat == Stat::none)
            continue;
        result[stat] = el.value();
    }
    return result;
}

json Stat::Serialize(const std::map<Stat::Id, int>& stats)
{
    auto result = json::object();
    for(const auto& stat: stats)
    {
        result[Stat::Name(stat.first)] = stat.second;
    }
    return result;
}

std::map<Stat::Id, std::string_view> Stat::statNames =
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
    { Stat::heat_resist, "heat resistance" },
    { Stat::cold_resist, "cold resistance" },
    { Stat::lightning_resist, "lightning resistance" },
    { Stat::poison_resist, "poison resistance" },

    { Stat::blunt_damage, "blunt damage" },
    { Stat::sharp_damage, "sharp damage" },
    { Stat::heat_damage, "heat damage" },
    { Stat::cold_damage, "cold damage" },
    { Stat::lightning_damage, "lightning damage" },
    { Stat::poison_damage, "poison damage" }
};
}

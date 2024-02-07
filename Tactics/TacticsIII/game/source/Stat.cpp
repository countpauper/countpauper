#include "Game/Stat.h"
#include "Game/Statted.h"
#include "Game/StatDefinition.h"
#include <nlohmann/json.hpp>
#include <optional>

namespace Game
{

Stat::Stat() = default;

Stat::Stat(std::string_view name, std::string_view description)
        : name(name)
        , description(description)
{
}

Stat::Stat(std::string_view name, std::string_view description, Id dependency, std::initializer_list<int> table, int multiplier)
        : name(name)
        , description(description)
        , dependency(dependency)
        , table(table)
        , multiplier(multiplier)
{
}

Stat::Stat(std::string_view name, std::string_view description, Stat::Id dependency,
        Stat::Operator op, Stat::Id operand)
        : name(name)
        , description(description)
        , dependency(dependency)
        , op(op)
        , operand(operand)
{
}

template<typename T>
std::optional<T> GetMaybe(const json& j, std::string_view key)
{
        auto it = j.find(key);
        if (it == j.end())
        {
                return std::optional<T>();
        }
         else
        {
                return it.value().template get<T>();
        }
}

template<typename T>
T GetOr(const json& j, std::string_view key, T&& alternative=T())
{
        auto it = j.find(key);
        if (it == j.end()) {
                return std::move(alternative);
        } else {
                return it.value().template get<T>();
        }
}

void Stat::Load(const json& j, const StatDefinition& dependencies)
{       // TODO: this is pretty clost to ns::from_json
        description = GetOr(j, "description", std::string());
        if (auto dependName = GetMaybe<std::string>(j, "Depends"))
        {
                this->dependency = dependencies.Find(*dependName);
        }
}

StatDescriptor Stat::Compute(const Statted& object) const
{
        auto left = 0;
        if (dependency)
        {
                left = object.Get(dependency).Total();      // TODO: add this to a STatDescriptor output ?
        }
        if (!table.empty())
        {
                // TODO table[0] is base, which doesnt matter much except for StatDescriptor
                // especially dodge 50% is just base and the rest 5% is 50+25[agi]
                // HD might be 5+2[con]
                if (left > table.size())
                {
                        left = table.back();
                }
                else if (left <= 0)
                {
                        left = table.front();
                }
                else
                {
                        left = table[left];
                }
        }
        StatDescriptor result(range);
        if (op && operand)
        {
                auto right = object.Get(operand).Total();
                switch(op)
                {
                        case Multiply:
                                result.Contribute(object.Definition().at(dependency).name, left * right * multiplier);
                                break;
                        case Add:
                                result.Contribute(object.Definition().at(dependency).name, left * multiplier);
                                result.Contribute(object.Definition().at(operand).name, right * multiplier);
                                break;
                        default:
                                break;
                }
        } else {
                result.Contribute(object.Definition().at(dependency).name, left * multiplier);
        }
        return result;
}

std::string_view Stat::Name() const
{
        return name;
}


}

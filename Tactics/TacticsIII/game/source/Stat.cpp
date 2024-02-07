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

        // TODO: this can be refactored
        description = GetOr(j, "description", std::string());
        if (auto dependName = GetMaybe<std::string>(j, "Depends"))
        {
                dependency = dependencies.Find(*dependName);
        } else {
                dependency = Stat::none;
        }
        table = GetOr(j, "table", std::vector<int>());

        auto limitVector = GetOr(j, "range", std::vector<int>());
        if (limitVector.size() >= 2)
        {
                limit.begin = limitVector.front();
                limit.end = limitVector.back();
        }

        op = Operator::nop;
        operand = Stat::none;
        auto addIt = j.find("+");
        if (addIt != j.end())
        {
                op = Operator::add;
                operand = dependencies.Find(addIt.value().template get<std::string>());
        }
        auto mulIt = j.find("*");
        if (mulIt != j.end())
        {
                const auto& operandValue = mulIt.value();
                if (operandValue.is_number_integer())
                {
                        // would just add it to the table then?
                        operandValue.get_to(multiplier);
                        operand = Stat::none;
                }
                else if (operandValue.is_string())
                {
                        assert(operand == Stat::none);  // already used by add?
                        operand = dependencies.Find(operandValue.template get<std::string>());
                        multiplier = 1;
                }

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
        StatDescriptor result(limit);
        if (op && operand)
        {
                auto right = object.Get(operand).Total();
                switch(op)
                {
                        case multiply:
                                result.Contribute(object.Definition().at(dependency).name, left * right * multiplier);
                                break;
                        case add:
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

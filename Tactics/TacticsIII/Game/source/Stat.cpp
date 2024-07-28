#include "Game/Stat.h"
#include "Game/Statted.h"
#include "Game/StatDefinition.h"
#include "Game/Counter.h"
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
Stat::Stat(std::string_view name, const json& j, const StatDefinition& dependencies) :
        name(name)
{       // TODO: this is pretty close to ns::from_json

        // TODO: this can be refactored
        description = Engine::get_value_or(j, "description", std::string());
        if (auto dependName = Engine::try_get<std::string>(j, "depends"))
        {
                dependency = dependencies.Identify(*dependName);
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
                operand = dependencies.Identify(*addOperand);
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
                        operand = dependencies.Identify(mulOperand->template get<std::string>());
                        multiplier = 1;
                        op = Stat::multiply;
                }
        }
}

Stat::~Stat() = default;

StatDescriptor Stat::Compute(const Statted& object) const
{
        auto left = 0;
        if (dependency)
        {
                left = object.Get(dependency).Total();      // TODO: add this to a StatDescriptor output ?
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
        }
        else if (dependency)
                result.Contribute(object.Definition().at(dependency).name, left * multiplier);
        else
        {
                // NB this could cause infinite recursion, unless object is mocked or its a primary stat
                auto mockStat = object.Get(object.Definition().Identify(Name()));
                if (mockStat.IsValid())
                            result.Contribute("Mock", mockStat.Total());

        }
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

}

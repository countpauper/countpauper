#include "Game/Stat.h"
#include "Game/Statted.h"
#include "Game/StatDefinition.h"
#include "Game/Counter.h"
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
                        op = Operator::multiply;
                }
        }
        resricted = Restrictions::Parse(j, "if");
}

Stat::~Stat() = default;

Computation Stat::Compute(const Statted& object, const Restrictions& restricted) const
{
        Computation result(limit);
        if (dependency)
        {
                result += object.Get(dependency, resricted);
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
                auto right = object.Get(operand, resricted);
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


}

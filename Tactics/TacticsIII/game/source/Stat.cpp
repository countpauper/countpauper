#include "Game/Stat.h"
#include "Game/Character.h"

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

StatDescriptor Stat::Compute(const Character& c) const
{
        auto left = 0;
        if (dependency)
        {
                left = c.Stat(dependency).Total();      // TODO: add this to a STatDescriptor output ?
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
                auto right = c.Stat(operand).Total();
                switch(op)
                {
                        case Multiply:
                                result.Contribute(Character::definition[dependency].name, left * right * multiplier);
                                break;
                        case Add:
                                result.Contribute(Character::definition[dependency].name, left * multiplier);
                                result.Contribute(Character::definition[operand].name, right * multiplier);
                                break;
                        default:
                                break;
                }
        } else {
                result.Contribute(Character::definition[dependency].name, left * multiplier);
        }
        return result;
}

}

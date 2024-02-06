#include "Game/Character.h"

namespace Game
{

Character::Character(std::string_view name, const Race& race) :
        name(name),
        race(race),
        primaryStats{
                {Stat::Level, 1},
                {Stat::Str, 2},
                {Stat::Agi, 2},
                {Stat::Con, 2},
                {Stat::Wis, 2},
                {Stat::Int, 2}
        }
{

}

void Character::Level(Stat::Id stat, int amount)
{
        primaryStats[stat] += amount;
}

StatDescriptor Character::Stat(Stat::Id id) const
{
        StatDescriptor result;

        auto it = primaryStats.find(id);
        int base = 0;

        if (it!=primaryStats.end())
        {
                result.Contribute("", it->second, false);
        }
        else
        {
                result = definition[id].Compute(*this);
        }
        // TODO: add all other
        result.Contribute(race.Name(), race.Bonus(id));
        return result;
}

StatDefinition Character::definition;

}

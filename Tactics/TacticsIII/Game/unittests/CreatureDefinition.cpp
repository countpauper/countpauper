#include "CreatureDefinition.h"
#include "Game/Creature.h"
#include <cmath>


namespace Game::Test
{

CreatureDefinition::CreatureDefinition()
{
    Creature::definition.clear();
}

CreatureDefinition& CreatureDefinition::Define(Stat::Id stat, Stat::Id dependency, float multiplier)
{
    Ensure(dependency);
    if (dependency == Stat::Id::none)
    {
            Creature::definition[stat] = Stat(Creature::definition.Name(stat));
    }
    else if (multiplier == std::floor(multiplier))
    {
        Creature::definition[stat] = Stat(Creature::definition.Name(stat), std::string_view(), dependency, {}, int(multiplier));
    }
    else
    {
        std::initializer_list<int> table={
            int(0*multiplier),
            int(1*multiplier),
            int(2*multiplier),
            int(3*multiplier),
            int(4*multiplier),
            int(5*multiplier),
            int(6*multiplier),
            int(7*multiplier),
            int(8*multiplier),
            int(9*multiplier),
            int(10*multiplier)
        };
        Creature::definition[stat] = Stat(Creature::definition.Name(stat), std::string_view(), dependency, table, 1);
    }
    last = stat;
    return *this;
}

CreatureDefinition& CreatureDefinition::Define(Stat::Id stat, Stat::Id dependency, Stat::Operator op, Stat::Id operand)
{
    Ensure(dependency);
    Ensure(operand);
    Creature::definition[stat] = Stat(Creature::definition.Name(stat), std::string_view(), dependency, op, operand);
    last = stat;
    return *this;
}

CreatureDefinition& CreatureDefinition::Count()
{
    Creature::definition.Count(last);
    return *this;
}

void CreatureDefinition::Ensure(Stat::Id id)
{
    if ((id != Stat::Id::none) && (Creature::definition.count(id)==0))
    {
        Creature::definition[id] = Stat(Creature::definition.Name(id), std::string_view());
    }
}

}

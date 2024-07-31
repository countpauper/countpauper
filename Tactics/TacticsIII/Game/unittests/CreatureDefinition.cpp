#include "CreatureDefinition.h"
#include "Game/Creature.h"
#include <cmath>


namespace Game::Test
{

Definition::Definition(StatDefinition& def) :
    definition(def)
{
    definition.clear();
}

Definition& Definition::Define(Stat::Id stat, Stat::Id dependency, float multiplier)
{
    Ensure(dependency);
    if (dependency == Stat::Id::none)
    {
            definition[stat] = Stat(definition.Name(stat));
    }
    else if (multiplier == std::floor(multiplier))
    {
        definition[stat] = Stat(definition.Name(stat), std::string_view(), dependency, {}, int(multiplier));
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
        definition[stat] = Stat(definition.Name(stat), std::string_view(), dependency, table, 1);
    }
    last = stat;
    return *this;
}

Definition& Definition::Define(Stat::Id stat, Stat::Id dependency, Stat::Operator op, Stat::Id operand)
{
    Ensure(dependency);
    Ensure(operand);
    definition[stat] = Stat(definition.Name(stat), std::string_view(), dependency, op, operand);
    last = stat;
    return *this;
}

Definition& Definition::Define(Stat::Id stat, int value)
{
    definition[stat] = Stat(definition.Name(stat), std::string_view(), Stat::Id::none, {value});
    last = stat;
    return *this;
}

Definition& Definition::Count()
{
    definition.Count(last);
    return *this;
}

void Definition::Ensure(Stat::Id id)
{
    if ((id != Stat::Id::none) && (definition.count(id)==0))
    {
        definition[id] = Stat(definition.Name(id), std::string_view());
    }
}


CreatureDefinition::CreatureDefinition() :
    Definition(Creature::definition)
{
}

}

#include "Definition.h"
#include "Game/Creature.h"
#include <cmath>


namespace Game::Test
{

Definition::Definition(StatDefinition& def) :
    definition(def)
{
    definition.clear();
}

Definition& Definition::Define(Stat::Id stat, Stat::Id dependency, float multiplier, const Restrictions& restricted)
{
    Ensure(dependency);
    if (dependency == Stat::Id::none)
    {
            definition[stat] = Stat(Stat::Name(stat), "", restricted);
    }
    else if (multiplier == std::floor(multiplier))
    {
        definition[stat] = Stat(Stat::Name(stat), std::string_view(), dependency, {}, int(multiplier), restricted);
    }
    else
    {
        int table[] {
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
        definition[stat] = Stat(Stat::Name(stat), std::string_view(), dependency, table, 1, restricted);
    }
    last = stat;
    return *this;
}

Definition& Definition::Define(Stat::Id stat, Stat::Id dependency, Operator op, Stat::Id operand, const Restrictions& restricted)
{
    Ensure(dependency);
    Ensure(operand);
    definition[stat] = Stat(Stat::Name(stat), std::string_view(), dependency, op, operand, restricted);
    last = stat;
    return *this;
}

Definition& Definition::Define(Stat::Id stat, int value, const Restrictions& restricted)
{
    int table[1] = { value };
    definition[stat] = Stat(Stat::Name(stat), std::string_view(), Stat::Id::none, table, 1, restricted);
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
        definition[id] = Stat(Stat::Name(id), std::string_view());
    }
}


CreatureDefinition::CreatureDefinition() :
    Definition(Creature::definition)
{
}

}

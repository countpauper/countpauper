#include "Game/Creature.h"
#include "Game/Unarmed.h"

namespace Game
{

Creature::Creature(std::string_view name, const Race& race) :
    Statistics({
        {Stat::level, 1},
        {Stat::str, 2},
        {Stat::agi, 2},
        {Stat::con, 2},
        {Stat::wis, 2},
        {Stat::intel, 2}
    }),
    Counters(static_cast<Statted&>(*this)),
    name(name),
    race(race)
{
    InitializeCounters();
    Equip(Equipment(race.GetUnarmed()));
}


std::string_view Creature::Name() const
{
    return name;
}

const Race& Creature::GetRace() const
{
    return race;
}

StatDescriptor Creature::Get(Stat::Id id) const
{
    // Get primary stat
    StatDescriptor result = Statistics::Get(id);
    // Get item stat
    if (!result.IsValid())
    {
        result = GetItemStat(id);
    }
    // Compute secondary stat
    if ((!result.IsValid()) && (id))
    {
        auto it = definition.find(id);
        if (it!=definition.end())
        {
            result = it->second.Compute(*this);
        }
    }

    if (result.IsValid())
    {
        // Add bonuses
        result = Conditions::Contribute(id, result);
        // TODO: add all other bonuses from items and magic
        result.Contribute(race.Name(), race.Bonus(id));
    }
    return result;
}

const StatDefinition& Creature::Definition() const
{
    return Creature::definition;
}


void Creature::OnCount(Stat::Id stat, unsigned remaining)
{
    if (stat == Stat::hp && remaining == 0)
    {
        Apply<Downed>();
        Apply<KO>();
    }
}

StatDefinition Creature::definition;

}

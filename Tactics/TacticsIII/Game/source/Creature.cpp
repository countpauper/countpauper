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

Computation Creature::Get(Stat::Id id, const Restrictions& restrict) const
{
    // Get primary stat
    Computation result = Statistics::Get(id, restrict);
    // Get item stat
    if (!result)
    {
        result = Equipments::Get(id, restrict);
    }
    // Compute secondary stat
    if ((result.empty()) && (id))
    {
        auto it = definition.find(id);
        if (it!=definition.end())
        {
            result = it->second.Compute(*this, restrict);
        }
    }

    if (!result.empty())
    {
        // Add bonuses
        result += Conditions::Boni(id);
        // TODO: add all other bonuses from items and magic
        result += race.Bonus(id);
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

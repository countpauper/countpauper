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
    inventory.emplace_back(Equipment(race.GetUnarmed()));
}


std::string_view Creature::Name() const
{
    return name;
}

const Race& Creature::GetRace() const
{
    return race;
}

bool Creature::Unequip(const Equipment& item)
{
    auto it = std::find_if(inventory.begin(), inventory.end(), [&item](const Equipment& e)
    {
        return &e == &item;
    });
    if (it == inventory.end())
        return false;
    inventory.erase(it);
    return true;
}

unsigned Creature::Unequip(const Restrictions filter)
{
    return std::erase_if(inventory, [filter](const Equipment& e)
    {
        return e.GetItem().Match(filter);
    });
}

void Creature::Equip(const Equipment& equipment)
{
    Restrictions exclude = equipment.GetItem().Exclusive();
    Unequip(exclude);
    inventory.emplace_back(std::move(equipment));
}

StatDescriptor Creature::Get(Stat::Id id) const
{
    // Get primary stat
    StatDescriptor result = Statistics::Get(id);
    // Get item stat
    if (!result.IsValid())
    {
        for(const auto& equipment : inventory)
        {
            result = equipment.Get(id);
            if (result.IsValid())
            {
                break;
            }
        }
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

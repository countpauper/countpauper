#include "Game/Creature.h"
#include "Game/Unarmed.h"
#include "Game/Boni.h"

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
    tags |= Restriction::creature;
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

// TODO: this is a bit hacky and/or it would have to pass original extraBoni, restrictions but more importantly items need to get
// the correct creature stat (damage/magic/none) for offense based on the item. It's not up to this class to decide how they map
class ItemBoni : public Boni
{
public:
    explicit ItemBoni(const Creature& creature) :
        creature(creature)
    {
    }
    Computation Bonus(Stat::Id stat) const
    {
        return creature.Get(stat, nullptr, {Restriction::creature});
    }
protected:
    const Creature& creature;
};

Computation Creature::Get(Stat::Id id, const Game::Boni* extraBoni, const Restrictions& restrict) const
{
    if (!restrict.Match(tags))
        return Computation(0, Name());

    // Get primary stat
    Computation result = Statistics::Get(id, extraBoni, restrict);
    // Get item stat
    if (!result)
    {
        ItemBoni boni(*this); // TODO: also pass extraBoni? what are they? player or group or location boni?
        result = Equipments::Get(id, &boni, restrict);
    }
    // Compute secondary stat
    if ((result.empty()) && (id))
    {
        auto it = definition.find(id);
        if (it!=definition.end())
        {
            result = it->second.Compute(*this, extraBoni, restrict);
        }
    }

    if (!result.empty())
    {
        // Add bonuses
        result += Conditions::Boni(id);
        // TODO: add all other bonuses from items and magic
        result += race.Bonus(id);
        if (extraBoni)
            result += extraBoni->Bonus(id);
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

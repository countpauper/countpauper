#include "Game/Creature.h"
#include "Game/Unarmed.h"
#include "Game/Boni.h"
#include "Game/ParentBoni.h"

namespace Game
{

Creature::Creature(std::string_view name, const Race& race) :
    Statistics({
        {Stat::level, 1},
        {Stat::str, 2},
        {Stat::agi, 2},
        {Stat::con, 2},
        {Stat::wis, 2},
        {Stat::intel, 2},
        {Stat::hands, 2}
    }),
    Counters(definition, static_cast<Statted&>(*this)),
    name(name),
    race(race)
{
    tags |= Restriction::creature;
    Equip(Equipment(race.GetUnarmed()));
}

Creature::Creature(const Race& race, const class ItemDatabase& items, const json& data) :
    Statistics(definition, Engine::must_get<json>(data, "stats")),
    Counters(definition, static_cast<Statted&>(*this), data),
    Conditions(Engine::get_value_or<json>(data, "conditions")),
    Equipments(items, Engine::get_value_or<json>(data, "equipment")),
    name(Engine::must_get<std::string_view>(data, "name")),
    race(race)
{
}

std::string_view Creature::Name() const
{
    return name;
}

Position Creature::GetPosition() const
{
    return position;
}

Engine::Size Creature::GetSize() const
{
    return race.GetSize();
}

const Position& Creature::SetPosition(Position pos)
{
    return position = pos;
}

const Race& Creature::GetRace() const
{
    return race;
}

json Creature::Serialize() const
{
    json result =  json::object({
        {"name", Name()},
        {"position", json::array({position.p.x, position.p.y, position.z_offset + position.p.z})},
        {"race", race.Name()},
        {"stats", Statistics::Serialize()},
        {"counters", Counters::Serialize()},
        {"conditions", Conditions::Serialize()},
        {"equipment", Equipments::Serialize()}

    });
    return result;
}


Computation Creature::Get(Stat::Id id, const Game::Boni* extraBoni, const Restrictions& restrict) const
{
    if (!restrict.Match(tags))
    {
        ParentBoni boni(*this, extraBoni, restrict ^ Restriction::creature);
        return Equipments::Get(id, &boni, restrict);
    }
    // Get primary stat
    Computation result = Statistics::Get(id, extraBoni, restrict);
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
        // Add own bonuses
        result += Bonus(id);
        if (extraBoni)
            result += extraBoni->Bonus(id);
    }
    return result;
}

Computation Creature::Bonus(Stat::Id id) const
{
    return race.Bonus(id) + Equipments::Bonus(id) + Conditions::Bonus(id);
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

const Equipment& Creature::Equip(const Equipment& equipment)
{
    assert(equipment.CanEquip(*this));
    return Equipments::Equip(equipment);
}

StatDefinition Creature::definition;

}

#include "Game/Item.h"
#include "Game/Boni.h"
#include "Game/Creature.h"

namespace Game
{

using json = nlohmann::json;


Item::Item(std::string_view name, Restrictions tags, Stat::Id offenseBonus, Stats stats) :
    Statistics(stats),
    name(name),
    tags(tags),
    offenseBonus(offenseBonus)
{
    tags |= Restriction::item;
}

Item::Item(const Item& other) :
    Statistics(other),
    name(other.name),
    tags(other.tags),
    offenseBonus(other.offenseBonus),
    bonus(other.bonus)
{
}

bool Item::operator==(const Item& o) const
{
    return name == o.name &&
        tags == o.tags &&
        stats == o.stats;
}


Item::Item(const json& data) :
    Statistics(definition, data),
    name(Engine::must_get<std::string_view>(data, "name")),
    tags(Restrictions::Parse(data, "tags"))
{
    auto offenseBonusStr = Engine::get_value_or<std::string_view>(data,"stat", "none");
    offenseBonus = Stat::Identify(offenseBonusStr);
    // TODO: load non item stats as bonuses
}

std::string_view Item::Name() const
{
    return name;
}

Computation Item::Get(Stat::Id id, const class Boni* extraBoni, const Restrictions& restricted) const
{
    if (!Match(restricted))
        return Computation(0, Name());

    // Get primary stat
    Computation result = Statistics::Get(id, extraBoni, restricted);
    // Compute secondary stat
    if ((result.empty()) && (id))
    {
        auto it = definition.find(id);
        if (it!=definition.end())
        {
            result = it->second.Compute(*this, extraBoni, restricted);
        }
        if (result)
        {
            if (extraBoni)
                result += extraBoni->Bonus(id);
        }
    }
    // Add Creature's damage or magic to weapon offense
    if ((id == Stat::offense) && (offenseBonus!=Stat::none) && (extraBoni))
    {
        result += extraBoni->Bonus(offenseBonus);
    }
    return result;
}

bool Item::Match(const Restrictions& restrictions) const
{
    return restrictions.Match(tags);
}

Restrictions Item::Swap() const
{
    if (tags.Contains(Restriction::melee))
    {
        return Restrictions{Restriction::unarmed, Restriction::ranged};
    }
    else if (tags.Contains(Restriction::unarmed))
    {
        return Restrictions{Restriction::melee, Restriction::ranged};
    }
    else if (tags.Contains(Restriction::ranged))
    {
        return Restrictions{Restriction::unarmed, Restriction::melee};
    }
    else if (tags.Contains(Restriction::armor))
    {
        return Restrictions{Restriction::armor};
    }
    else if (tags.Contains(Restriction::shield))
    {
        return Restrictions{Restriction::shield};
    }
    else
    {
        return Restrictions({Restriction::none});
    }
}

Restriction Item::GetMaterial() const
{
    auto materials = GetTags() & Restrictions::material;
    return materials.at(0);
}

Restrictions Item::GetTags() const
{
    return tags;
}


StatDefinition Item::definition;

const StatDefinition& Item::Definition() const
{
    return definition;
}


};

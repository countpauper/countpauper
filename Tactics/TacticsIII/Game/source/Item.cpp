#include "Game/Item.h"

namespace Game
{

using json = nlohmann::json;


Item::Item(std::string_view name, Restrictions tags) :
    name(name),
    tags(tags)
{
}

bool Item::operator==(const Item& o) const
{
    return name == o.name &&
        tags == o.tags &&
        stats == o.stats;
}


Item::Item(const json& data) :
    name(Engine::must_get<std::string_view>(data, "name")),
    tags(ParseRestrictions(data, "tags"))
{
    Statistics::Load(data);
    // TODO: load non item stats as bonuses
}

std::string_view Item::Name() const
{
    return name;
}

Computation Item::Get(Stat::Id id) const
{
    // Get primary stat
    Computation result = Statistics::Get(id);
    // Compute secondary stat
    if ((result.empty()) && (id))
    {
        auto it = definition.find(id);
        if (it!=definition.end())
        {
            result = it->second.Compute(*this);
        }
    }
    return result;
}

bool Item::Match(const Restrictions& restrictions) const
{
    return Game::Match(tags, restrictions);
}

Restrictions Item::Excludes() const
{
    if (Match({Restriction::melee}))
    {
        return Restrictions{Restriction::unarmed, Restriction::ranged};
    }
    else if (Match({Restriction::unarmed}))
    {
        return Restrictions{Restriction::melee, Restriction::ranged};
    }
    else if (Match({Restriction::ranged}))
    {
        return Restrictions{Restriction::unarmed, Restriction::melee};
    }
    else if (Match({Restriction::armor}))
    {
        return Restrictions{Restriction::armor};
    }
    else if (Match({Restriction::shield}))
    {
        return Restrictions{Restriction::shield};
    }
    else
    {
        return Restrictions({Restriction::none});
    }
}

StatDefinition Item::definition;

const StatDefinition& Item::Definition() const
{
    return definition;
}


};

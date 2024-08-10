#include "Game/Item.h"
#include "Game/Boni.h"

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
    tags(Restrictions::Parse(data, "tags"))
{
    Statistics::Load(data);
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

    return result;
}

bool Item::Match(const Restrictions& restrictions) const
{
    return restrictions.Match(tags);
}

Restrictions Item::Excludes() const
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

StatDefinition Item::definition;

const StatDefinition& Item::Definition() const
{
    return definition;
}


};

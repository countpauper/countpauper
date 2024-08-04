#include "Game/Item.h"

namespace Game
{

using json = nlohmann::json;
Item::Item() = default;


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

StatDescriptor Item::Get(Stat::Id id) const
{
    return Statistics::Get(id);
}

bool Item::Match(const Restrictions& restrictions) const
{
    return Game::Match(tags, restrictions);
}


StatDefinition Item::definition;

const StatDefinition& Item::Definition() const
{
    return definition;
}


};

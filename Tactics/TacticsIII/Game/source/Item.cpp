#include "Game/Item.h"

namespace Game
{

Item::Item() = default;

std::string_view Item::Name() const
{
    return name;
}

StatDescriptor Item::Get(Stat::Id id) const
{
    return Statted::Get(id);
}

StatDefinition Item::definition;

const StatDefinition& Item::Definition() const
{
    return definition;
}


};

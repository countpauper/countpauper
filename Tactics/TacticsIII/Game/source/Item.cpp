#include "Game/Item.h"

namespace Game
{

Item::Item() = default;

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

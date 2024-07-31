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

unsigned Item::Available(Stat::Id) const
{
    return 0; // TODO item chages
}

unsigned Item::Cost(Stat::Id counter, unsigned cost, bool truncate)
{
    return 0;
}

void Item::Reset(Counter::Reset at)
{
}


};

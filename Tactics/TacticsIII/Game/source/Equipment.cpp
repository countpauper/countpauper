#include "Game/Equipment.h"

namespace Game
{

Equipment::Equipment(const Item& item) :
    Counters(item),
    item(&item)
{
}

Equipment::Equipment(const Equipment& o) :
    Counters(o),
    item(o.item)
{
}

Equipment::Equipment(Equipment&& o) :
    Counters(std::move(o)),
    item(o.item)
{
}

Equipment& Equipment::operator=(const Equipment& o)
{
    item = o.item;
    Counters::operator=(o);
    return *this;
}

Equipment& Equipment::operator=(Equipment&& o)
{
    item = nullptr;
    std::swap(item, o.item);
    return *this;
}

Computation Equipment::Get(Stat::Id id, const Restrictions& restricted) const
{
    // TODO: add material and bonus
    return item->Get(id, restricted);
}

const Item& Equipment::GetItem() const
{
    return *item;
}

std::string Equipment::Name() const
{
    return std::string(item->Name());    // TODO: add pre and postfixes of materials and bonus
}

}

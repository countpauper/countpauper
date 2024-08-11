#include "Game/Equipment.h"

namespace Game
{

Equipment::Equipment(const Item& item, std::vector<ItemBonus> boni) :
    Counters(item),
    item(&item),
    boni(boni)
{
}

Equipment::Equipment(const Equipment& o) :
    Counters(o),
    item(o.item),
    boni(o.boni)
{
}

Equipment::Equipment(Equipment&& o) :
    Counters(std::move(o)),
    item(o.item),
    boni(o.boni)
{
    o.boni.clear();
    o.item = nullptr;
}

Equipment& Equipment::operator=(const Equipment& o)
{
    item = o.item;
    boni = o.boni;
    Counters::operator=(o);
    return *this;
}

Equipment& Equipment::operator=(Equipment&& o)
{
    item = nullptr;
    boni.clear();
    std::swap(item, o.item);
    std::swap(boni, o.boni);
    return *this;
}

Computation Equipment::Get(Stat::Id id, const class Boni* extraBoni, const Restrictions& restricted) const
{
    // TODO: add material and bonus and creature bonus but how are those added to dependency stats
    auto result = item->Get(id, extraBoni, restricted);
    for(const auto& bonus: boni)
        result += bonus.Bonus(id);
    return result;
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

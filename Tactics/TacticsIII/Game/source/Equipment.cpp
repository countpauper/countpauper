#include "Game/Equipment.h"
#include "Utility/String.h"

namespace Game
{

Equipment::Equipment(const Item& item, std::vector<const ItemBonus*> boni) :
    Counters(Item::definition, item),
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
        result += bonus->Bonus(id);
    return result;
}

const Item& Equipment::GetItem() const
{
    return *item;
}

std::string Equipment::Name() const
{
    std::string result;
    std::vector<std::string_view> nameParts;
    for(const auto& bonus: boni)
    {
        auto prefix = bonus->Prefix();
        if (!prefix.empty())
            nameParts.push_back(prefix);
    }
    nameParts.push_back(item->Name());
    for(const auto& bonus: boni)
    {
        auto postfix = bonus->Postfix();
        if (!postfix.empty())
            nameParts.push_back(postfix);
    }
    return Engine::Join(nameParts, " ");
}


void Equipment::Deserialize(const json& data)
{

}

json Equipment::Serialize() const
{
    auto modifiers = json::array();
    for(const auto& bon: boni)
    {
        modifiers.push_back(bon->Name());
    }
    auto result = json::object({
        {"item", item->Name() },
        {"counters", Counters::Serialize()},
        {"modifiers", modifiers}
    });
    return result;
}

}

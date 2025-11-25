#include "Game/Equipment.h"
#include "Utility/String.h"
#include "Game/ItemDatabase.h"
#include "Game/Creature.h"

namespace Game
{

Equipment::Equipment(const Item& item, std::vector<const ItemBonus*> boni) :
    Counters(Item::definition, item),
    item(&item),
    boni(boni)
{
}

std::vector<const ItemBonus*> DeserializeBoni(const ItemDatabase& items, const Restrictions& tags, const json& data)
{
    std::vector<const ItemBonus*>  result;
    for(const auto& bonuData : data)
    {
        auto bonusName = bonuData.get<std::string_view>();
        auto materialPtr = items.FindBonus(tags | Restriction::material, bonusName);
        auto bonusPtr =  items.FindBonus(tags | Restriction::bonus, bonusName);
        bonusPtr.insert(bonusPtr.begin(), materialPtr.begin(), materialPtr.end());  // insert materials first, they have priority

        if (bonusPtr.empty())
            throw std::runtime_error(std::string("Bonus ") + std::string(bonusName) + " is not found in the database");
        if (bonusPtr.size()!=1)
            throw std::runtime_error(std::string("Ambiguous bonus ") + std::string(bonusName) + " in the database. " + std::to_string(bonusPtr.size()) + " found");

        result.push_back(bonusPtr.front());
    }
    return result;
}

Equipment::Equipment(const ItemDatabase& items, const Item& item, const json& data) :
    Counters(Item::definition, item),
    item(&item),
    boni(DeserializeBoni(items, item.GetTags(), Engine::get_value_or<json>(data, "modifiers", json())))
{
}

Equipment::Equipment(const ItemDatabase& items, const json& data) :
    Equipment(items, items.Get(Engine::must_get<std::string_view>(data, "item")), data)
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

Computation Equipment::Bonus(Stat::Id id) const
{
    Computation sum;
    for(auto bonus : boni)
    {
        sum += bonus->Bonus(id);
    }
    return sum.Simplify();
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

struct Limit
{
    Stat::Id itemCost;
    Restrictions itemFilter;
    Stat::Id creatureCapacity;
};


Requirements Equipment::CanEquip(const Statted& stats) const
{
    const auto* boni = dynamic_cast<const Boni*>(&stats);

    static const Limit limits[]={
        {Stat::hold, Restrictions(), Stat::hands},
        {Stat::weight, Restrictions::weapon, Stat::lift},
        {Stat::weight, Restrictions({Restriction::armor}), Stat::endurance},
        {Stat::enchantment, Restrictions(), Stat::attune}
    };
    auto unequip = item->Swap();
    Requirements result;
    for(const auto& limit: std::span(limits))
    {
        if (!item->Match(limit.itemFilter))
            continue;
        auto required = stats.Get(limit.itemCost, boni, limit.itemFilter);
        required -= stats.Get(limit.itemCost, boni, unequip);
        required += Get(limit.itemCost, boni, limit.itemFilter);  // TODO can this preexisting bonus be found/multiplied by 0 instead?
        auto available = stats.Get(limit.creatureCapacity);
        result.emplace_back(StatRequirement{ limit.itemCost, available, Comparator::greater_equal, required});
    }
    return result;
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

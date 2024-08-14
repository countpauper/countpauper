#pragma once

#include "Game/Item.h"
#include "Game/ItemBonus.h"
#include <vector>

namespace Game
{

class ItemDatabase
{
public:
    ItemDatabase() = default;
    // Equipment depends on the item and bonus pointers. They should not be copied. Moving should be possible but is not yet implemented.
    ItemDatabase(const ItemDatabase&) = delete;
    ItemDatabase& operator=(const ItemDatabase&) = delete;
    explicit ItemDatabase(const json& data);
    std::vector<const Item*> Find(Restrictions filter) const;
    const Item* Find(std::string_view name) const;
    std::vector<const ItemBonus*> FindBonus(const Restrictions& filter, std::string_view name="") const;
private:
    std::vector<Item> items;
    std::vector<ItemBonus> boni;
};

}

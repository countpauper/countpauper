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
    explicit ItemDatabase(const json& data);
    const Item* Find(std::string_view name) const;
    std::vector<const ItemBonus*> FindBonus(const Restrictions& filter, std::string_view name="") const;
private:
    std::vector<Item> items;
    std::vector<ItemBonus> boni;
};

}

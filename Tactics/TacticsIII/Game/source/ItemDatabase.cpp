#include "Game/ItemDatabase.h"
#include "Utility/String.h"
#include <nlohmann/json.hpp>
#include "File/Json.h"
#include <iostream>
#include <fstream>

namespace Game
{
using json = nlohmann::json;

std::vector<Item> Parse(const json& data, std::string_view section)
{
    std::vector<Item> result;
    const auto& sectionData = Engine::must_get<json>(data, section);
    for(const auto& itemData : sectionData)
    {
        result.emplace_back(itemData);
    }
    return result;
}

std::vector<ItemBonus> ParseBoni(const json& data, std::string_view section)
{
    std::vector<ItemBonus> result;
    const auto& sectionData = Engine::must_get<json>(data, section);
    for(const auto& bonusData : sectionData)
    {
        result.emplace_back(bonusData);
    }
    return result;
}

ItemDatabase::ItemDatabase(const json& data)
{
    auto weapons = Parse(data, "weapon");
    items = weapons;
    auto armors = Parse(data, "armor");
    items.insert(items.end(), armors.begin(), armors.end());

    weaponMaterial = ParseBoni(data, "weapon_material");
    weaponBonus = ParseBoni(data, "weapon_bonus");
    armorMaterial = ParseBoni(data, "armor_material");
    armorBonus = ParseBoni(data, "armor_bonus");
}

const Item* ItemDatabase::Find(std::string_view name) const
{
    for(const auto& item : items)
    {
        if (item.Name() == name)
            return &item;
    }
    return nullptr;
}

}

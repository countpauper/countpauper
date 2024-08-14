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

std::vector<ItemBonus> ParseBoni(const json& data, std::string_view section, const Restrictions& tags)
{
    std::vector<ItemBonus> result;
    const auto& sectionData = Engine::must_get<json>(data, section);
    for(const auto& bonusData : sectionData)
    {
        auto& bonus = result.emplace_back(bonusData, tags);
    }
    return result;
}

ItemDatabase::ItemDatabase(const json& data)
{
    Item::definition.Parse(Engine::LoadJson("data/item_stats.json"));

    items = Parse(data, "weapon");
    const auto& armors = Parse(data, "armor");
    items.insert(items.end(), armors.begin(), armors.end());

    boni = ParseBoni(data, "bonus", {Restriction::bonus});
    const auto& weaponMaterials = ParseBoni(data, "weapon_material", Restrictions::weapon | Restriction::material);
    boni.insert(boni.end(), weaponMaterials.begin(), weaponMaterials.end());
    const auto& armorMaterial = ParseBoni(data, "armor_material", {Restriction::armor, Restriction::material});
    boni.insert(boni.end(), armorMaterial.begin(), armorMaterial.end());
}

std::vector<const Item*> ItemDatabase::Find(Restrictions filter) const
{
    std::vector<const Item*> result;
   for(const auto& item : items)
        if (item.Match(filter))
            result.push_back(&item);
    return result;
}
const Item* ItemDatabase::Find(std::string_view name) const
{
    for(const auto& item : items)
        if (item.Name() == name)
            return &item;
    return nullptr;
}

std::vector<const ItemBonus*> ItemDatabase::FindBonus(const Restrictions& filter, std::string_view name) const
{
    std::vector<const ItemBonus*> result;
    for(const auto& bonus : boni)
        if ((bonus.Match(filter)) &&
            (bonus.NameMatch(name)))
            result.push_back(&bonus);
    return result;
}


}

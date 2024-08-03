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

void ItemDatabase::Load(std::istream& s)
{
    json parsed = json::parse(s);
    auto weapons = Parse(parsed, "weapon");
    items = weapons;
    auto armors = Parse(parsed, "armor");
    items.insert(items.end(), armors.begin(), armors.end());
}

void ItemDatabase::Load(const char* filename)
{
    std::ifstream dbFile(filename);
    Load(dbFile);
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

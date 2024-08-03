#pragma once

#include "Game/Item.h"
#include <vector>

namespace Game
{



class ItemMaterial
{
public:

private:

};

class Bonus
{
public:

private:

};

class ItemDatabase
{
public:
    void Load(std::istream& data);
    void Load(const char* filename);
    const Item* Find(std::string_view name) const;
private:
    std::vector<Item> items;
    std::vector<ItemMaterial> materials;
    std::vector<Bonus> boni;
};

}

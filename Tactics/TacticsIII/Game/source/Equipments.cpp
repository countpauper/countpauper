#include "Game/Equipments.h"

namespace Game
{

bool Equipments::Unequip(const Equipment& item)
{
    auto it = std::find_if(equipped.begin(), equipped.end(), [&item](const Equipment& e)
    {
        return &e == &item;
    });
    if (it == equipped.end())
        return false;
    equipped.erase(it);
    return true;
}

unsigned Equipments::Unequip(const Restrictions filter)
{
    return std::erase_if(equipped, [filter](const Equipment& e)
    {
        return e.GetItem().Match(filter);
    });
}

const Equipment& Equipments::Equip(const Equipment& equipment)
{
    Restrictions exclude = equipment.GetItem().Excludes();
    Unequip(exclude);
    return equipped.emplace_back(std::move(equipment));
}

Computation Equipments::Get(Stat::Id id, const class Boni* extraBoni, const Restrictions& filter) const
{
    Computation result = Item::definition.GetPrimaryStat(id);
    if (!result.empty())
    {
        return result;
    }
    for(const auto& e : equipped)
    {
        if (e.GetItem().Match(filter))
        {
            Computation itemResult = e.Get(id, extraBoni, filter);
            result += itemResult;
        }
    }
    return result;
}

std::vector<const Equipment*> Equipments::GetEquipped(const Restrictions& filter) const
{
    std::vector<const Equipment*> result;
    result.reserve(equipped.size());

    std::transform(equipped.begin(), equipped.end(), std::back_inserter(result),
        [](const Equipment& e)
    {
        return &e;
    });
    std::erase_if(result, [filter](const Equipment* e)
    {
        return !e->GetItem().Match(filter);
    });
    result.shrink_to_fit();
    return result;
}

void Equipments::Deserialize(const json& data)
{
}

json Equipments::Serialize() const
{
    auto result = json::array();
    for(const auto& equipment : equipped)
    {
        result.push_back(equipment.Serialize());
    }
    return result;
}

}

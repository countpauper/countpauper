#include "Game/Equipped.h"

namespace Game
{

bool Equipped::Unequip(const Equipment& item)
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

unsigned Equipped::Unequip(const Restrictions filter)
{
    return std::erase_if(equipped, [filter](const Equipment& e)
    {
        return e.GetItem().Match(filter);
    });
}

void Equipped::Equip(const Equipment& equipment)
{
    Restrictions exclude = equipment.GetItem().Excludes();
    Unequip(exclude);
    equipped.emplace_back(std::move(equipment));
}

StatDescriptor Equipped::GetItemStat(Stat::Id id, Restrictions filter) const
{
    StatDescriptor result;
    // TODO: could check if it's a valid stat in Item::definition
    // TODO: context (main hand attack, offhand attack, attacked from the front or not)
    for(const auto& e : equipped)
    {
        if (e.GetItem().Match(filter))
        {
            result = e.Get(id);
            if (result.IsValid())   // TODO: add multiple armor bonuses for shueld+armor if applicable
                return result;
        }
    }
    return result;
}

}

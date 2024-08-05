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

void Equipments::Equip(const Equipment& equipment)
{
    Restrictions exclude = equipment.GetItem().Excludes();
    Unequip(exclude);
    equipped.emplace_back(std::move(equipment));
}

StatDescriptor Equipments::GetItemStat(Stat::Id id, const Restrictions& filter) const
{
    StatDescriptor result = Item::definition.GetPrimaryDescriptor(id);
    if (!result.IsValid())
    {
        return result;
    }
    for(const auto& e : equipped)
    {
        if (e.GetItem().Match(filter))
        {
            StatDescriptor itemResult = e.Get(id);
            if (itemResult.IsValid())
            {
                if (result.IsValid())
                {
                    result.Contribute(e.Name(), itemResult.Total());
                }
                else
                {
                    result = itemResult;
                }
            }
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

}

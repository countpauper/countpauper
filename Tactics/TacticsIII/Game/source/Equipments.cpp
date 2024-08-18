#include "Game/Equipments.h"
#include "Game/ItemDatabase.h"
#include <cassert>

namespace Game
{

Equipments::Equipments(const ItemDatabase& db, const json& data)
{
    for(const auto& el: data)
    {
        equipped.emplace_back(db, el);
    }
}

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

Computation Equipments::GetTotal(Stat::Id stat, const Restrictions& include, const Restrictions& exclude) const
{
    Computation sum;
    for(const auto& e : equipped)
    {
        if (!e.GetItem().Match(include))
            continue;
        if (e.GetItem().Match(exclude))
            continue;
        sum += Computation(e.Get(stat).Total(), e.Name());
    }
    return sum.Simplify();
}

Computation Equipments::Bonus(Stat::Id id) const
{
    Computation sum;
    for(const auto& e : equipped)
    {
        sum += e.Bonus(id);
    }
    return sum.Simplify();
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
            if (itemResult)
                result += Computation(itemResult.Total(), e.Name());
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

#include "Game/ItemBonus.h"
#include "Game/Item.h"
#include "File/Json.h"

namespace Game
{

ItemBonus::ItemBonus(const json& data) :
    prefix(Engine::get_value_or<std::string_view>(data, "prefix", "")),
    postfix(Engine::get_value_or<std::string_view>(data, "postfix", ""))
{
    stats = Item::definition.LoadStats(data);
}

Computation ItemBonus::Bonus(Stat::Id stat) const
{
    auto it = stats.find(stat);
    if (it == stats.end())
        return Computation();
    return Computation(it->second);
}

}

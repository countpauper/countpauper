#include "Game/ItemBonus.h"
#include "Game/Item.h"
#include "File/Json.h"

namespace Game
{

ItemBonus::ItemBonus(const json& data, const Restrictions& extraTags) :
    prefix(Engine::get_value_or<std::string_view>(data, "prefix", "")),
    postfix(Engine::get_value_or<std::string_view>(data, "postfix", "")),
    restrictions(Restrictions::Parse(data, "restrictions"))
{
    stats = Stat::LoadStats(data);
    restrictions |= extraTags;
}

Computation ItemBonus::Bonus(Stat::Id stat) const
{
    auto it = stats.find(stat);
    if (it == stats.end())
        return Computation();
    return Computation(it->second);
}

bool ItemBonus::Match(const Restrictions& tags) const
{
    return restrictions.Match(tags);
}

bool ItemBonus::NameMatch(std::string_view name) const
{
    return (prefix.find(name)!=std::string::npos || postfix.find(name)!=std::string::npos);
}

const std::string_view ItemBonus::Prefix() const
{
    return prefix;
}

const std::string_view ItemBonus::Postfix() const
{
    return postfix;
}

}

#include "Game/Statted.h"
#include "Game/StatDefinition.h"

namespace Game
{

Statted::Statted(std::initializer_list<std::pair<const Stat::Id, int>> stats) :
    stats(stats)
{
}

StatDescriptor Statted::Get(Stat::Id id) const
{
    auto it = stats.find(id);
    if (it==stats.end())
    {
        return StatDescriptor();
    }
    StatDescriptor result(Definition().at(id).Limit());
    result.Contribute("", it->second, false);
    return result;
}

}

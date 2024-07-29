#include "Game/Condition.h"

namespace Game
{

Condition::Condition(std::string_view name) :
    name(name)
{
}

std::string_view Condition::Name() const
{
    return name;
}

int Condition::Contribution(Stat::Id stat) const
{
    auto it = bonus.find(stat);
    if (it!=bonus.end())
        return it->second;
    else
        return 0;
}


KO::KO() :
    Condition("KO")
{
    bonus[Stat::ap] = -255;
}

Downed::Downed() :
    Condition("Downed")
{
}

}

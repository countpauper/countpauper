#include "Game/Condition.h"

namespace Game
{

Condition::Condition(std::string_view name) :
    name(name)
{
}

Condition::Condition(std::string_view name, const json& data) :
    Condition(name)
{
    bonus = Stat::Deserialize(data);
}


std::string_view Condition::Name() const
{
    return name;
}

Computation Condition::Bonus(Stat::Id stat) const
{
    auto it = bonus.find(stat);
    if (it!=bonus.end())
        return Computation(it->second, Name());
    else
        return Computation();
}

std::pair<std::string_view, json> Condition::Serialize() const
{
    return std::make_pair(name, Stat::Serialize(bonus));
}

std::unique_ptr<Condition> Condition::Deserialize(std::string_view name, const json& data)
{
    if (name=="KO")
    {
        return std::make_unique<KO>();
    }
    else if (name=="Downed")
    {
        return std::make_unique<Downed>();
    }
    else
    {
        assert(false); // unimplemented subtype
        return std::make_unique<Condition>(name, data);
    }

}


KO::KO() :
    Condition("KO")
{
    bonus[Stat::ap] = std::numeric_limits<int>::min();
}

Downed::Downed() :
    Condition("Downed")
{
    bonus[Stat::speed] = -2;
    bonus[Stat::dodge] = -5;
}

}

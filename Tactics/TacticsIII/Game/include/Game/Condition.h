#pragma once
#include <string_view>
#include <map>
#include "Game/Stat.h"

namespace Game
{

class Condition
{
public:
    Condition() = default;
    virtual ~Condition() = default;
    std::string_view Name() const;
    Computation Bonus(Stat::Id stat) const;
protected:
    Condition(std::string_view name);
    std::string_view name;
    std::map<Stat::Id, int> bonus;
};

class KO : public Condition
{
public:
    KO();
};

class Downed : public Condition
{
public:
    Downed();
};

}

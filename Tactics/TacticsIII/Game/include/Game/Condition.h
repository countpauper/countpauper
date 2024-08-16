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
    Condition(std::string_view name, const json& data);
    virtual ~Condition() = default;
    std::string_view Name() const;
    Computation Bonus(Stat::Id stat) const;
    std::pair<std::string_view, json> Serialize() const;
    static std::unique_ptr<Condition> Deserialize(std::string_view name, const json& data);
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

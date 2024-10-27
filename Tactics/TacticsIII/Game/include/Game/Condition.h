#pragma once
#include <string_view>
#include <map>
#include "Game/Stat.h"
#include "Game/Boni.h"

namespace Game
{

class Condition : public Boni
{
public:
    Condition() = default;
    Condition(std::string_view name, const json& data);
    virtual ~Condition() = default;
    std::string_view Name() const;
    Computation Bonus(Stat::Id stat) const;
    std::pair<std::string_view, json> Serialize() const;
    static std::unique_ptr<Condition> Deserialize(std::string_view name, const json& data);

    static const Condition* Find(std::string_view name);
protected:
    Condition(std::string_view name);
    std::string_view name;
    std::map<Stat::Id, int> bonus;
    static std::set<const Condition*> all;
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

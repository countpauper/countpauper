#pragma once
#include "Game/StatDefinition.h"
#include "Game/Statistics.h"
#include "Game/Counter.h"
#include "Game/Restriction.h"
#include <map>
#include <string>
#include <nlohmann/json.hpp>
#include <iostream>

namespace Game
{

class Item :
    public Statistics
{
public:
    explicit Item(std::string_view name="", Restrictions tags={}, Stat::Id offenseBonus = Stat::none, Stats stats=Stats());
    explicit Item(const nlohmann::json& data);
    Item(const Item& other);
    static StatDefinition definition;
    std::string_view Name() const override;
    Computation Get(Stat::Id id, const class Boni* extraBoni = nullptr, const Restrictions& restricted={}) const;
    const StatDefinition& Definition() const;
    bool Match(const Restrictions& restictions) const;
    Restrictions Swap() const;
    Restriction GetMaterial() const;
    Restrictions GetTags() const;
    bool operator==(const Item& o) const;
protected:
    std::string name;
    Restrictions tags;
    Stat::Id offenseBonus;
    std::map<Stat::Id, int> bonus;
};

}

#pragma once
#include "Game/StatDefinition.h"
#include "Game/Statistics.h"
#include "Game/Counter.h"
#include "Game/Restriction.h"
#include <map>
#include <string>


namespace Game
{

class Item :
    public Statistics
{
public:
    explicit Item(std::string_view name="", Restrictions tags={});
    explicit Item(const nlohmann::json& data);
    static StatDefinition definition;
    std::string_view Name() const override;
    Computation Get(Stat::Id id) const;
    const StatDefinition& Definition() const;
    bool Match(const Restrictions& restictions) const;
    Restrictions Excludes() const;
    bool operator==(const Item& o) const;
protected:
    std::string name;
    Restrictions tags;
    std::map<Stat::Id, int> bonus;
};

}

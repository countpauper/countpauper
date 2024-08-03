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
    Item();
    explicit Item(const nlohmann::json& data);
    static StatDefinition definition;
    std::string_view Name() const override;
    StatDescriptor Get(Stat::Id id) const;
    const StatDefinition& Definition() const;
protected:
    std::string name;
    Restrictions restrictions;
    std::map<Stat::Id, int> bonus;
};

}

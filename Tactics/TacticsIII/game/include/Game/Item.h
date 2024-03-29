#pragma once
#include "Game/StatDefinition.h"
#include "Game/Statted.h"
#include <map>
#include <string>

namespace Game
{

class Item :
    public Statted
{
public:
    Item();
    static StatDefinition definition;
    StatDescriptor Get(Stat::Id id) const;
    const StatDefinition& Definition() const;
protected:
    std::string name;
    std::map<Stat::Id, int> bonus;
    std::map<Stat::Id, int> enchantment;
};

}

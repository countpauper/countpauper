#pragma once
#include "Game/StatDefinition.h"
#include "Game/Statted.h"
#include "Game/Counter.h"
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
    std::string_view Name() const override;
    StatDescriptor Get(Stat::Id id) const;
    const StatDefinition& Definition() const;
    // Counters
    unsigned Available(Stat::Id) const override;
    unsigned Cost(Stat::Id counter, unsigned cost, bool truncate=false) override;
    void Reset(Counter::Reset at) override;
protected:
    std::string name;
    std::map<Stat::Id, int> bonus;
};

}

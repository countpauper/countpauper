#pragma once
#include "Game/StatDescriptor.h"
#include "Game/Stat.h"
#include <initializer_list>

namespace Game
{


using StatsDescriptor = std::map<Stat::Id, StatDescriptor>;

class Statted
{
public:
    Statted() = default;
    ~Statted() = default;
    virtual std::string_view Name() const = 0;
    virtual StatDescriptor Get(Stat::Id id) const = 0;
    virtual StatsDescriptor Get(std::initializer_list<Stat::Id> stats) const;

    virtual const class StatDefinition& Definition() const = 0;
    virtual void Level(Stat::Id stat, int amount) = 0;
};

}

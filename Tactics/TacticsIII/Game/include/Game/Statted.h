#pragma once
#include "Game/StatDescriptor.h"
#include "Game/Stat.h"

namespace Game
{

class Statted
{
public:
    Statted() = default;
    ~Statted() = default;
    virtual std::string_view Name() const = 0;
    virtual StatDescriptor Get(Stat::Id id) const = 0;
    virtual const class StatDefinition& Definition() const = 0;
    virtual void Level(Stat::Id stat, int amount) = 0;
};

}

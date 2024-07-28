#pragma once
#include "Game/StatDescriptor.h"
#include "Game/Stat.h"
#include <initializer_list>

namespace Game
{


class Statted
{
public:
    Statted() = default;
    Statted(std::initializer_list<std::pair<const Stat::Id, int>> stats);
    virtual std::string_view Name() const = 0;
    virtual StatDescriptor Get(Stat::Id id) const;
    virtual const class StatDefinition& Definition() const = 0;
protected:
    std::map<Stat::Id, int> stats;
};

}

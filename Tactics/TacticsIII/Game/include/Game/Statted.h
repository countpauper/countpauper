#pragma once
#include "Game/Computation.h"
#include "Game/Stat.h"
#include "Game/Restriction.h"
#include <initializer_list>

namespace Game
{


using Computations = std::map<Stat::Id, Computation>;

class Statted
{
public:
    Statted() = default;
    ~Statted() = default;
    virtual std::string_view Name() const = 0;
    virtual Computation Get(Stat::Id id, const Restrictions& restricted={}) const = 0;
    virtual Computations Get(std::initializer_list<Stat::Id> stats, const Restrictions& restricted={}) const;

    virtual const class StatDefinition& Definition() const = 0;
    virtual void Level(Stat::Id stat, int amount) = 0;
};

}

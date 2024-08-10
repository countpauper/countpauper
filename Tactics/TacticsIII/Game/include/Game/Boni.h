#pragma once
#include "Game/Stat.h"
#include "Game/Computation.h"

namespace Game
{

class Boni
{
public:
    virtual Computation Bonus(Stat::Id id) const = 0;
};

}

#pragma once
#include "Game/StatDescriptor.h"
#include "Game/Stat.h"

namespace Game
{


class Statted
{
public:
        virtual StatDescriptor Get(Stat::Id id) const = 0;
        virtual const class StatDefinition& Definition() const = 0;
};

}

#pragma once
#include "Game/Boni.h"
#include "Game/Restriction.h"

namespace Game
{

class ParentBoni : public Boni
{
public:
    ParentBoni(const Statted& parent, const Boni* extraBoni, Restrictions restrict);
    Computation Bonus(Stat::Id stat) const;
protected:
    const Statted& parent;
    const Boni* extraBoni;
    Restrictions restrict;
};


}

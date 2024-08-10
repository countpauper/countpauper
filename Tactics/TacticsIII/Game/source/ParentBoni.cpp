#include "Game/ParentBoni.h"
#include "Game/Statted.h"

namespace Game
{

ParentBoni::ParentBoni(const Statted& parent, const Boni* extraBoni, Restrictions restrict) :
    parent(parent),
    extraBoni(extraBoni),
    restrict(restrict)
{
}
Computation ParentBoni::Bonus(Stat::Id stat) const
{
    return parent.Get(stat, extraBoni, restrict);
}

}

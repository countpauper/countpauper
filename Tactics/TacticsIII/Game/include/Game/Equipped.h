#pragma once

#include "Game/Equipment.h"

namespace Game
{


class Equipped
{
public:
    virtual ~Equipped() = default;
    virtual const Equipment& Equip(const Equipment& equipment) = 0;
    virtual bool Unequip(const Equipment& item) = 0;
    virtual unsigned Unequip(const Restrictions filter) = 0;
    virtual Computation GetTotal(Stat::Id stat, Restrictions exclude) const = 0;
    virtual std::vector<const Equipment*> GetEquipped(const Restrictions& filter={}) const = 0;
    virtual Computation Get(Stat::Id id, const class Boni* extraBoni = nullptr, const Restrictions& filter=Restrictions()) const = 0;
};

}

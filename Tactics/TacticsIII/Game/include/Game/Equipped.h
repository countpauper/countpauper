#pragma once

#include "Game/Equipment.h"

namespace Game
{
class Equipped
{
public:
    Equipped() = default;
    void Equip(const Equipment& equipment);
    bool Unequip(const Equipment& item);
    unsigned Unequip(const Restrictions filter);
    StatDescriptor GetItemStat(Stat::Id id, Restrictions filter=Restrictions()) const;
protected:
    std::vector<Equipment> equipped;
};

}

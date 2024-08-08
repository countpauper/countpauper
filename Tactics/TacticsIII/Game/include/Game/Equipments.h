#pragma once

#include "Game/Equipped.h"

namespace Game
{

class Equipments : public Equipped
{
public:
    Equipments() = default;
    void Equip(const Equipment& equipment) override;
    bool Unequip(const Equipment& item) override;
    unsigned Unequip(const Restrictions filter) override;
    std::vector<const Equipment*> GetEquipped(const Restrictions& filter={}) const override;
    Computation GetItemStat(Stat::Id id, const Restrictions& filter={}) const override;
protected:
    std::vector<Equipment> equipped;
};

}

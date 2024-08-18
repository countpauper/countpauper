#pragma once
#include "Game/Item.h"
#include "Game/Counters.h"
#include "Game/ItemBonus.h"
#include "Game/Boni.h"
#include <map>
#include <string>


namespace Game
{

class Equipment :
    public Counters,
    public Boni
{
public:
    explicit Equipment(const Item& item, std::vector<const ItemBonus*> boni={});
    Equipment(const class ItemDatabase& db, const json& data);
    Equipment(const Equipment& o);
    Equipment(Equipment&& o);
    Equipment& operator=(const Equipment& o);
    Equipment& operator=(Equipment&& o);

    Computation Get(Stat::Id id, const class Boni* extraBoni = nullptr, const Restrictions& restricted={}) const;
    Computation Bonus(Stat::Id id) const override;

    const Item& GetItem() const;
    std::string Name() const;
    bool CanEquip(const Statted& stats) const;
    json Serialize() const;
protected:
    Equipment(const ItemDatabase& items, const Item& item, const json& data) ;
    std::vector<Stat::Id> EquipLimits(const Statted& stats) const;

    const Item* item;
    std::vector<const ItemBonus*> boni;
};

}

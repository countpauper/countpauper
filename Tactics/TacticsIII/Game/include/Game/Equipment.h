#pragma once
#include "Game/Item.h"
#include "Game/Counters.h"
#include "Game/ItemBonus.h"
#include <map>
#include <string>


namespace Game
{

class Equipment :
    public Counters
{
public:
    explicit Equipment(const Item& item, std::vector<ItemBonus> boni={});
    Equipment(const Equipment& o);
    Equipment(Equipment&& o);
    Equipment& operator=(const Equipment& o);
    Equipment& operator=(Equipment&& o);

    Computation Get(Stat::Id id, const class Boni* extraBoni = nullptr, const Restrictions& restricted={}) const;
    const Item& GetItem() const;
    std::string Name() const;
protected:
    const Item* item;
    std::vector<ItemBonus> boni;
};

}

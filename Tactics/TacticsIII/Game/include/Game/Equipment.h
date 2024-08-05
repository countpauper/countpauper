#pragma once
#include "Game/Item.h"
#include "Game/Counters.h"
#include <map>
#include <string>


namespace Game
{

class Equipment :
    public Counters
{
public:
    explicit Equipment(const Item& item);
    Equipment(const Equipment& o);
    Equipment(Equipment&& o);
    Equipment& operator=(const Equipment& o);
    Equipment& operator=(Equipment&& o);

    StatDescriptor Get(Stat::Id id) const;
    const Item& GetItem() const;
    std::string Name() const;
protected:
    const Item* item;
};

}

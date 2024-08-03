#pragma once
#include "Game/StatDefinition.h"
#include "Game/Statistics.h"
#include "Game/Counter.h"
#include <map>
#include <string>

namespace Game
{

enum class Restrictions
{
    none = 0,
    // material
    cloth,
    leather,
    metal,
    wood,
    // element
    air,
    stone,
    fire,
    nature,
    water,
    // damage (that is not an element)
    sharp,
    blunt,
    cold,
    poison,
    lightning,
    // armor type
    armor,
    shield,
    // weapon type
    melee,
    ranged,
    thrown
};

class Item :
    public Statistics
{
public:
    Item();
    static StatDefinition definition;
    std::string_view Name() const override;
    StatDescriptor Get(Stat::Id id) const;
    const StatDefinition& Definition() const;
protected:
    std::string name;
    std::map<Stat::Id, int> bonus;
};

}

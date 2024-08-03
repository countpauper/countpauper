#pragma once
#include <set>

namespace Game
{

enum class Restriction
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
    // base type
    weapon,
    armor,
    shield,
    // weapon type
    melee,
    ranged,
    thrown
};
using Restrictions = std::set<Restriction>;

}

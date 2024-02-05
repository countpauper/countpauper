#pragma once
#include <string>
#include "Game/Stat.h"
#include "Game/Race.h"
#include "Game/Item.h"

namespace Game
{

class Character
{
public:
        Character();

        // Each stat is first found in the primary stats of this character or else in the derived stats
        // either way, afterwards modifiers are applied of
        //  - race
        //  - knowledge
        //  - mundane items (range, damage, -hands -weight)
        //  - item enchantments
        //  - spells
        // If multiple spells or enchantments increase the same stat, only the highest is applied (of each or either?)
        // If multiple splles or enchantments decrease the same stat, only the lowest is applied (of each or either ?)
        // No stat can go over its maximum and no stat can go under 0 or its clipped
        int Stat(Stat::Id) const;
private:
        std::string name;
        Race& race;
        std::vector<Item&> inventory;
        std::map<Bonus::Id, int> primaryStats;  // level, str, agi,
        // TODO: knowledge, skills, actions, effects
};

}

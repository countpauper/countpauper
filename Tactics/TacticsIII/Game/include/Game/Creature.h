#pragma once
#include <string>
#include <functional>
#include "Game/Counters.h"
#include "Game/Statistics.h"
#include "Game/StatDefinition.h"
#include "Game/Race.h"
#include "Game/Item.h"
#include "Game/Conditions.h"
#include "UI/Object.h"

namespace Game
{

class Creature :
    public Statistics,
    public Counters,
    public Conditions
{
public:
    Creature(std::string_view name, const Race& race);

    // Each stat is first found in the primary stats of this character or else in the derived stats
    // either way, afterwards modifiers are applied of
    //  - race
    //  - knowledge
    //  - mundane items (range, damage, -hands -weight)
    //  - item enchantments
    //  - spells
    // If multiple spells or enchantments increase the same stat, only the highest is applied (of each or either?)
    // If multiple splles or enchantments decrease the same stat, only the lowest is applied (of each or either ?)
    // No stat can go over its maximum and no stat can go under 0 or its clipped (actually resistance could be negative)
    StatDescriptor Get(Stat::Id id) const override;
    const StatDefinition& Definition() const override;
    std::string_view Name() const override;
    const Race& GetRace() const;
    static StatDefinition definition;

private:
    void OnCount(Stat::Id stat, unsigned remaining) override;

    std::string name;
    const Race& race;
    std::vector<std::reference_wrapper<Item>> inventory;
    std::vector<std::unique_ptr<Item>> items;  // TODO: item db should be externally owned

    // TODO: effects(bonuses) knowledge (with bonuses), actions, skills(extra actions)

    // Loaded from JSON each Stat is defined by a name and which bonuses it gives to other stats at a certain level.
    // This level has to be recursively queried from the character along with all its bonuses
    // percentages are in whole %

};

}

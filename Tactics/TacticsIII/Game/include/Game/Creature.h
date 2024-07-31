#pragma once
#include <string>
#include <functional>
#include "Game/Statted.h"
#include "Game/StatDefinition.h"
#include "Game/Race.h"
#include "Game/Item.h"
#include "Game/Condition.h"
#include "Game/StatDescriptor.h"
#include "UI/Object.h"

namespace Game
{

class Creature :
    public Statted
{
public:
    Creature(std::string_view name, const Race& race);

    void Level(Stat::Id stat, int amount);

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

    unsigned Available(Stat::Id) const override;
    unsigned Cost(Stat::Id counter, unsigned cost, bool truncate=false) override;
    void Reset(Counter::Reset at) override;

    template<class CT>
    bool Is() const
    {
        return std::find_if(conditions.begin(), conditions.end(), [](const std::unique_ptr<Condition>& c)
        {
            return dynamic_cast<const CT*>(c.get()) != nullptr;
        }) != conditions.end();
    }
    template<class CT>
    void Apply()
    {
        // TODO: power level that replaces
        if (!Is<CT>())
        {
            conditions.emplace_back(std::make_unique<CT>());
        }
    }
    std::vector<std::reference_wrapper<const Condition>> Conditions() const;
private:
    void OnCounter(Stat::Id stat, unsigned remaining);

    std::string name;
    const Race& race;
    std::vector<std::reference_wrapper<Item>> inventory;
    std::map<const Counter*, int> countersUsed;
    std::vector<std::unique_ptr<Item>> items;  // TODO: item db should be externally owned
    std::vector<std::unique_ptr<Condition>> conditions;

    // TODO: effects(bonuses) knowledge (with bonuses), actions, skills(extra actions)

    // Loaded from JSON each Stat is defined by a name and which bonuses it gives to other stats at a certain level.
    // This level has to be recursively queried from the character along with all its bonuses
    // percentages are in whole %

};

}

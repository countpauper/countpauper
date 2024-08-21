#pragma once
#include <string>
#include <functional>
#include "Game/Counters.h"
#include "Game/Statistics.h"
#include "Game/StatDefinition.h"
#include "Game/Race.h"
#include "Game/Conditions.h"
#include "Game/Equipments.h"
#include "Game/Boni.h"
#include "Geometry/Position.h"
#include "UI/Object.h"

namespace Game
{

class Creature :
    public virtual Boni,
    public Statistics,
    public Counters,
    public Conditions,
    public Equipments
{
public:
    Creature(std::string_view name, const Race& race);
    Creature(const Race& race, const class ItemDatabase& items, const json& data);

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
    Computation Get(Stat::Id id, const class Boni* extraBoni = nullptr, const Restrictions& restricted={}) const override;
    const StatDefinition& Definition() const override;
    std::string_view Name() const override;
    const Equipment& Equip(const Equipment& equipment) override;
    Computation Bonus(Stat::Id id) const override;
    Engine::Position GetPosition() const;
    const Engine::Position& SetPosition(Engine::Position pos);
    const Race& GetRace() const;
    json Serialize() const;
    static StatDefinition definition;
private:
    void OnCount(Stat::Id stat, unsigned remaining) override;
    std::string name;
    Engine::Position position;
    const Race& race;
    Restrictions tags;
    // TODO: effects(bonuses) knowledge (with bonuses), actions, skills(extra actions)

    // Loaded from JSON each Stat is defined by a name and which bonuses it gives to other stats at a certain level.
    // This level has to be recursively queried from the character along with all its bonuses
    // percentages are in whole %

};

}

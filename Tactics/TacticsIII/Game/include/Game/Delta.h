#pragma once

#include "Game/Actor.h"
#include "UI/Object.h"
#include "Game/Conditions.h"
#include "Game/Statted.h"
#include "Game/Counted.h"
#include "Game/Equipped.h"

namespace Game
{
class Delta :
    public Actor,
    protected Engine::Object,
    protected Conditions,
    protected Statted,
    protected Counted,
    protected Equipped
{
public:
    Delta(Actor& parent);
    Delta(const Delta& other);
    Delta& operator=(const Delta& other);

    void Apply();

    void Move(const class World& world, Engine::Position destination) override;
    Engine::Position Position() const override;
    Engine::Size Size() const override;
    const Engine::Object& GetAppearance() const override;
    std::string_view Name() const override;
    void Render() const override;
    Engine::Coordinate GetCoordinate() const override;
    Engine::Quaternion GetOrientation() const override;

    Statted& GetStats() override;
    const Statted& GetStats() const override;
    Computation Get(Stat::Id id, const class Boni* boni = nullptr, const Restrictions& restricted={}) const override;
    const class StatDefinition& Definition() const override;
    void Level(Stat::Id stat, int amount) override;

    Counted& GetCounts() override;
    const Counted& GetCounts() const override;
    unsigned Available(Stat::Id) const override;
    unsigned Cost(Stat::Id counter, unsigned cost, bool truncate=false) override;
    void Reset(Counter::Reset at) override;

    const class Equipped& GetEquipment() const override;
    class Equipped& GetEquipment() override;
    const Equipment& Equip(const Equipment& equipment) override;
    bool Unequip(const Equipment& item) override;
    unsigned Unequip(const Restrictions filter) override;
    std::vector<const Equipment*> GetEquipped(const Restrictions& filter={}) const override;

public:
    Actor* parent;
    const class World* world;
    Engine::Position position;
    std::map<Stat::Id, int> counterDelta;
};

}

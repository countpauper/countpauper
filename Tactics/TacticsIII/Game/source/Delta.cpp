#include "Game/Delta.h"
#include "Game/World.h"
#include "Game/MapItf.h"

namespace Game
{
Delta::Delta(Actor& parent) :
    parent(&parent),
    world(nullptr)
{
}

Delta::Delta(const Delta& other) :
    Conditions(other),
    Statted(other),
    Counted(other),
    Equipped(other),
    parent(other.parent),
    world(other.world),
    position(other.position),
    counterDelta(other.counterDelta),
    conditionOverride(other.conditionOverride)
{
}

Delta::Delta(Delta&& other) :
    Conditions(std::move(other)),
    Statted(std::move(other)),
    Counted(std::move(other)),
    Equipped(std::move(other)),
    parent(other.parent),
    world(other.world),
    position(other.position),
    counterDelta(std::move(other.counterDelta)),
    conditionOverride(std::move(other.conditionOverride))
{
}

Delta& Delta::operator=(const Delta& other)
{
    parent = other.parent;
    position = other.position;
    world = other.world;
    counterDelta = other.counterDelta;
    return *this;
}

const Engine::Object& Delta::GetAppearance() const
{
    return *this;
}

void Delta::Apply()
{
    if (world)
        parent->Move(*world, position);
    for(const auto& dCounter : counterDelta)
        parent->GetCounts().Cost(dCounter.first, -dCounter.second, true);
    parent->GetConditions().ApplyConditions(conditionOverride.GetConditions());

}

std::string_view Delta::Name() const
{
    return parent->GetAppearance().Name();
}

void Delta::Render() const
{
}

Engine::Coordinate Delta::GetCoordinate() const
{
    return parent->GetAppearance().GetCoordinate();
}

Engine::Quaternion Delta::GetOrientation() const
{
    return parent->GetAppearance().GetOrientation();
}


void Delta::Move(const class World& world, Position destination)
{
    this->world = &world;
    auto z = this->world->GetMap().GroundHeight(destination);
    position = Position(destination.X(), destination.Y(), z);
}

Position Delta::GetPosition() const
{
    return position;
}

Position Delta::GetSize() const
{
    assert(false); // TODO: local storage that overrides when shape changing
    return parent->GetSize();
}

Statted& Delta::GetStats()
{
    return *this;
}

const Statted& Delta::GetStats() const
{
    return *this;
}

Computation Delta::Get(Stat::Id id, const class Boni* boni, const Restrictions& restricted) const
{
    assert(false); // TODO: local storage that overrides ... when leveling?
    return parent->GetStats().Get(id, boni, restricted);
}

const class StatDefinition& Delta::Definition() const
{
    return parent->GetStats().Definition();
}

void Delta::Level(Stat::Id stat, int amount)
{
    assert(false); // TODO: raise
}

Counted& Delta::GetCounts()
{
    return *this;
}

const Counted& Delta::GetCounts() const
{
    return *this;
}

unsigned Delta::Available(Stat::Id id) const
{
    auto it = counterDelta.find(id);
    int delta = 0;
    if (it!=counterDelta.end())
        delta = it->second;
    return parent->GetCounts().Available(id) + delta;
}

unsigned Delta::Cost(Stat::Id counter, unsigned cost, bool truncate)
{
    if (truncate)
    {
        cost = std::min(cost, Available(counter));
    }
    counterDelta[counter] = counterDelta[counter] - cost;
    return cost;
}

void Delta::Reset(Counter::Reset at)
{
    assert(false); // TODO determine which counter deltas need to be removed as delta
}

const Equipped& Delta::GetEquipment() const
{
    return *this;
}

class Equipped& Delta::GetEquipment()
{
    return *this;
}

const Equipment& Delta::Equip(const Equipment& equipment)
{
    assert(false); // TODO: keep a list of new equipment
    return equipment;
}

bool Delta::Unequip(const Equipment& item)
{
    assert(false); // TODO: keep a list of old equipment
    return false;
}

unsigned Delta::Unequip(const Restrictions filter)
{
    assert(false); // TODO
    return 0;
}

std::vector<const Equipment*> Delta::GetEquipped(const Restrictions& filter) const
{
    assert(false); // TODO
    return {};
}

Conditions& Delta::GetConditions() 
{
    return *this;
}

const Conditions& Delta::GetConditions() const
{
    return *this;
}

unsigned Delta::GetCondition(std::function<bool(const Condition& condition)> pred) const
{
    auto over = conditionOverride.FindCondition(pred);
    if (over.first)
        return over.second;
    else 
        return parent->GetConditions().GetCondition(pred);
}

void Delta::SetCondition(const Condition& condition, unsigned level)
{
    return conditionOverride.InsertCondition(condition, level);
}

void Delta::ApplyConditions(Range conditions)
{
    for(const auto& condition: conditions)
    {
        conditionOverride.InsertCondition(*condition.first, condition.second);
    }
}

Computation Delta::ConditionalBonus(Stat::Id id) const
{
    assert(false); // TODO: compute from overriddn conditions
    // but then (the hard part) from the non-overridden conditions in the parent
    return 0;
}


}

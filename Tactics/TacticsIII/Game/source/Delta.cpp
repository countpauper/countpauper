#include "Game/Delta.h"

namespace Game
{
Delta::Delta(Actor& parent) :
    parent(&parent),
    world(nullptr)
{
}

Delta::Delta(const Delta& other) :
    parent(other.parent),
    world(other.world),
    position(other.position),
    counterDelta(other.counterDelta)
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


void Delta::Move(const class World& world, Engine::Position destination)
{
    this->world = &world;
    position = destination; // TODO: could be a Vector instead
}

Engine::Position Delta::Position() const
{
    return position;
}

Engine::Size Delta::Size() const
{
    assert(false); // TODO: local storage that overrides when shape changing
    return parent->Size();
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

}

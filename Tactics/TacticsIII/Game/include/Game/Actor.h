#pragma once
#include "Game/Position.h"
#include "UI/Object.h"

namespace Game
{

class Actor
{
public:
    virtual void Move(const class World& world, Position destination) = 0;
    virtual Position GetPosition() const = 0;
    virtual Size GetSize() const = 0;

    bool In(const Engine::IntBox& bounds) const;
    Engine::IntBox GetBounds() const;

    virtual const Engine::Object& GetAppearance() const = 0;
    virtual class Statted& GetStats() = 0;
    virtual const Statted& GetStats() const = 0;
    virtual class Counted& GetCounts() = 0;
    virtual const Counted& GetCounts() const = 0;
    virtual const class Equipped& GetEquipment() const = 0;
    virtual class Equipped& GetEquipment() = 0;
    virtual class Conditions& GetConditions() = 0;
    virtual const class Conditions& GetConditions() const = 0;
};






}

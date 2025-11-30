#pragma once
#include "Game/Statted.h"
#include "Game/Counted.h"
#include "Game/Position.h"
#include "Geometry/Size.h"
#include "UI/Object.h"

namespace Game
{

class Actor
{
public:
    virtual void Move(const class World& world, Engine::Position destination) = 0;
    virtual Position GetPosition() const = 0;
    virtual Engine::Size Size() const = 0;

    virtual const Engine::Object& GetAppearance() const = 0;
    virtual Statted& GetStats() = 0;
    virtual const Statted& GetStats() const = 0;
    virtual Counted& GetCounts() = 0;
    virtual const Counted& GetCounts() const = 0;
    virtual const class Equipped& GetEquipment() const = 0;
    virtual class Equipped& GetEquipment() = 0;

};

}

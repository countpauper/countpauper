#pragma once
#include "Game/Statted.h"
#include "Game/Counted.h"
#include "Geometry/Position.h"
#include "UI/Scenery.h"

namespace Game
{

class Actor
{
public:
    virtual void Move(const class Map& map, Engine::Position destination) = 0;

    virtual const Engine::Scenery& GetAppearance() const = 0;
    virtual Engine::Position Position() const = 0;
    virtual Statted& GetStats() = 0;
    virtual const Statted& GetStats() const = 0;
    virtual Counted& GetCounts() = 0;
    virtual const Counted& GetCounts() const = 0;
};

}

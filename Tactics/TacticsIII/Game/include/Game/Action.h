#pragma once
#include <string>
#include "Game/Requirement.h"
#include "Game/Delta.h"

namespace Game
{
class Actor;
class World;
class Action
{
public:
    Action(World& world, Actor& actor);
    virtual void Render() const = 0;
    virtual unsigned AP() const = 0;
    virtual Requirements CanDo() const = 0;
    virtual std::vector<Delta> Execute(std::ostream& log) const = 0;
    virtual std::string Description() const = 0;
protected:
    World& world;
    Actor& actor;
};



}

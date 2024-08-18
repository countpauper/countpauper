#pragma once
#include <string>
#include "Game/Requirement.h"

namespace Game
{
class Actor;
class Game;

class Action
{
public:
    Action(Actor& actor);
    virtual void Render() const = 0;
    virtual unsigned AP() const = 0;
    virtual Requirements CanDo() const = 0;
    virtual void Execute(std::ostream& log) const = 0;
    virtual std::string Description() const = 0;
protected:
    Actor& actor;
};

}

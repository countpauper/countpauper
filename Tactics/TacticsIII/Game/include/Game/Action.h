#pragma once
#include <string>
#include "Game/Requirement.h"
#include "Game/Delta.h"
#include "Game/TargetQueue.h"
#include <tuple>

namespace Game
{
class Actor;
class World;

template<typename AT>
using TargetsFor = typename ExtractTargets<typename AT::Targets>::type;

// TODO maybe make this a functor with a base class so it can be instantiated in the action selection
// or even better then, combine it with the TargetQueue 
// or even better than that, fuse it into its Action class Attack { static ActionFactory<Attack> factory; }
// and maybe then fuse it further by using CRTP class Attack : public Fabricatable<Attack> { }  
// or even better than that, make a generic factory utility then that takes the constructor parameters.
template<typename AT>
std::unique_ptr<AT> ActionFactory(World& world, Actor& actor, TargetsFor<AT> targets)
{
    std::apply(std::make_unique<AT>, std::tuple_cat(std::tie(world, actor), targets.as_tuple()));
}


class Action
{
public:
    Action(World& world, Actor& actor);
    virtual void Render() const = 0;
    virtual unsigned AP() const = 0;
    virtual Requirements CanDo() const = 0;
    virtual std::vector<Delta> Execute(std::ostream& log) const = 0;
    virtual std::string Description() const = 0;
    virtual Position GetDestination() const = 0;
    virtual unsigned GetDistance() const = 0;
protected:
    World& world;
    Actor& actor;
};



}

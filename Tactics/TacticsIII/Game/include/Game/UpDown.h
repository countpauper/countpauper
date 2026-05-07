#pragma once
#include "Game/ApplySelf.h"
#include "Game/PlanFactory.h"
namespace Game
{

class Up : public ApplySelf
{
public:
    using Targets=std::tuple<>;   
    using Factory = PlanFactoryAction<Up>;

    Up(World& world, Actor& actor);
    void Render() const override;
    unsigned AP() const override;
};

class Down : public ApplySelf
{
public:
    using Targets=std::tuple<>;   
    using Factory = PlanFactoryAction<Down>;

    Down(World& world, Actor& actor);
    void Render() const override;
    unsigned AP() const override;
};

}

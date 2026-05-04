#pragma once
#include "Game/ApplySelf.h"

namespace Game
{

class Up : public ApplySelf
{
public:
    using Targets=std::tuple<>;   
    Up(World& world, Actor& actor);
    void Render() const override;
    unsigned AP() const override;
};

class Down : public ApplySelf
{
public:
    using Targets=std::tuple<>;   
    Down(World& world, Actor& actor);
    void Render() const override;
    unsigned AP() const override;
};

}

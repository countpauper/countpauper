#pragma once
#include "Game/ApplySelf.h"

namespace Game
{

class Up : public ApplySelf
{
public:
    Up(World& world, Actor& actor);
    void Render() const override;
    unsigned AP() const override;
};

class Down : public ApplySelf
{
public:
    Down(World& world, Actor& actor);
    void Render() const override;
    unsigned AP() const override;
};

}

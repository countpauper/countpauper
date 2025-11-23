#pragma once

#include "Game/Orientation.h"
#include "Game/Material.h"

namespace Game
{

class Block
{
public:
    // TODO could be 16 bit fixed point to halve size or large maps of 256*256*64 = 4 mil grids
    // but better to optimize to binary tree into either detailed maps and and air /solid / single material blocks.
    using Amount = float;
    using Temperature = float;
    Block();
    Block(float air, float water, float veg, float rock);

    static const Block Water;
    static const Block Stone;
    static const Block Air;
    static Block Vegetation(float vegetation, float rock);
    void AddWater(float moreWater);
    const Material& GetMaterial(Orientation side) const;
    float LiquidLevel() const; // NaN is no liquid. 1.0 is full
    float SolidLevel() const;  // NaN is no ground. 1.0 is full solid
private:
    float air;  // if > 1.0-(rest) then it's pressurized. if < then underpressure (vacuum). In Bar
    Amount water;       // liquid level = rock + min(nature,water) + water
    Amount vegetation;  // if > water sticks out into air.
    Amount stone;       // level of stone (for now)
};



}

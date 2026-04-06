#include "Game/Block.h"
#include <limits>
#include <assert.h>

namespace Game
{


Block::Block()
    : temperature(0.0f)
    , air(0.0f)
    , water(0.0f)
    , vegetation(0.0f)
    , stone(0.0f)
{
}

Block::Block(float stone, float water, float veg, float temp)
    : temperature(temp)
    , air(1.0-(stone+water+veg))
    , water(water)
    , vegetation(veg)
    , stone(stone)
{
    if (water + vegetation + stone > 1.0f)
        throw std::invalid_argument("Block overloaded");
}

const Material& Block::GetMaterial(Orientation side) const
{
    if (side == Orientation::up)
    {
        if (water)
            return Material::water;
        else if (vegetation)
            return Material::vegetation;
        else if (stone)
            return Material::stone;
    }
    else if (side == Orientation::down)
    {
        if (stone)
            return Material::stone;
        else if (vegetation)
            return Material::vegetation;
        else if (water)
            return Material::water;
    }
    else
    {
        assert(false);
        // TODO return a vector<pair<const Material*, float>> stack
        // This can also be used for top and bottom, it just gets top (minus air) and bottom of that stack
        // although veg is tricky later as it will be a mesh and mixed materials/states will be needed (eg mud, oil, tile)
    }
    return Material::air;
}

float Block::SolidLevel() const
{
    if (stone==0.0f)
        return std::numeric_limits<float>::quiet_NaN();
    return stone;
}

float Block::LiquidLevel() const
{
    if (water == 0.0f )
        return std::numeric_limits<float>::quiet_NaN();
    auto waterVeg = std::min(vegetation, water);
    return stone + water + waterVeg;
}

float Block::AirPressure() const 
{
    float airVolume = 1.0 - (water+stone+vegetation);
    return air/airVolume;
}

//                          stone    water   veg     air is remaining
const Block Block::Air      (0.0,   0.0f,   0.0f);
const Block Block::Water    (0.0f,  1.0f,   0.0f);
const Block Block::Stone    (1.0f,  0.0f,   0.0f);
const Block Block::Space;

void Block::AddWater(float moreWater)
{
    if (moreWater>air)
    {
        air = 0.0f;
    } else {
        air -= moreWater;
    }
    water += moreWater;
}

bool Block::CanWalk() const
{
    auto solidLvl = SolidLevel();
    if (solidLvl>=1.0)
        return false; 
    if (solidLvl<=0.0)
        return false;
    if (LiquidLevel()>=1.0)
        return false;
    return true;
}


}

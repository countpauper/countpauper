#include "Game/Block.h"
#include <limits>
#include <assert.h>

namespace Game
{

Block::Block(float air, float water, float veg, float stone)
    : air(air)
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

//                          air     water   veg.    rock
const Block Block::Air      (1.0f,  0.0f,   0.0f,   0.0f);
const Block Block::Water    (0.0f,  1.0f,   0.0f,   0.0f);
const Block Block::Stone    (0.0f,  0.0f,   0.0f,   1.0f);

Block Block::Vegetation(float vegetation, float rock)
{
    return Block(1.0f - (vegetation+rock), 0.0f, vegetation, rock);
}


}

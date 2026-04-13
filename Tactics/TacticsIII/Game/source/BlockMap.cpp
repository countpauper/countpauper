#include "Game/BlockMap.h"
#include "Game/Block.h"
#include <cmath>

namespace Game
{

Engine::Coordinate BlockMap::GroundCoord(Engine::Position pos) const
{
    return Game::Position(pos.x, pos.y, GroundHeight(Position(pos))).Coord();
}

Slice BlockMap::GetSlice(Position pos, float height) const
{
    Slice slice; 
    float posz = 0;
    auto z_fraction = std::modf(pos.Z(), &posz);
    for(int z = 0; z<std::ceil(height + z_fraction); ++z) // TODO test this z_fraction, 
    {
        slice += Slice(GetBlock({pos.X(), pos.Y(), int(posz) + z}));
    }
    slice &= Engine::Range<float>(z_fraction, z_fraction + height);
    return slice;
}

float BlockMap::GroundHeight(Position pos) const
{
    auto top = ceil(pos.Z());
    top = GetBounds().z.Clip(top);

    for(int z=top; z>=0; --z)
    {
        auto block = GetBlock(Engine::Position(pos.X(), pos.Y(), z));
        auto height = block.SolidLevel();
        if (!std::isnan(height))
        {
            return height + z;
        }
    }
    return 0.0f;
}

}

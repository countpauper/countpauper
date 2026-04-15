#include "Game/BlockMap.h"
#include "Game/Block.h"
#include <cmath>

namespace Game
{

Engine::Coordinate BlockMap::GroundCoord(Engine::Position pos) const
{
    return Game::Position(pos.x, pos.y, GroundHeight(Position(pos))).Coord();
}

Slice BlockMap::GetSlice(Position pos, Position::ZType height) const
{
    Slice slice; 
    int posz = 0;
    auto z_fraction = modfp(pos.Z(), posz);
    for(int z = 0; z< ceil(height + z_fraction); ++z) // TODO test this z_fraction, 
    {
        slice += Slice(GetBlock({pos.X(), pos.Y(), posz + z}));
    }
    // TODO Slice with fixed point 
    slice &= Engine::Range<float>(static_cast<float>(z_fraction), static_cast<float>(z_fraction + height));
    return slice;
}

Position::ZType BlockMap::GroundHeight(Position pos) const
{
    auto top = ceil(pos.Z());
    top = GetBounds().z.Clip(top);

    for(int z=top; z>=0; --z)
    {
        auto block = GetBlock(Engine::Position(pos.X(), pos.Y(), z));
        auto height = block.SolidLevel();
        if (!std::isnan(height))
        {
            return Position::ZType(height + z); // TODO SolidLevel in std::optional<FIxedPoint>
        }
    }
    return 0.0f;
}

}

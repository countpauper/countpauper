#include "Game/BlockMap.h"
#include "Game/Block.h"


namespace Game
{

Engine::Coordinate BlockMap::GroundCoord(Engine::Position pos) const
{
    return Game::Position(pos.x, pos.y, GroundHeight(Position(pos))).Coord();
}

Slice BlockMap::GetSlice(Position pos, float height) const
{
    Slice slice; 
    for(int z = 0; z<std::ceil(height + pos.z_offset); ++z) // TODO test this z_offset, 
    {
        slice += Slice(GetBlock({pos.X(), pos.Y(), pos.p.z + z}));
    }
    slice &= Engine::Range<float>(pos.z_offset, pos.z_offset + height);
    return slice;
}

float BlockMap::GroundHeight(Position pos) const
{
    for(int z=ceil(pos.Z()); z>=0; --z)
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

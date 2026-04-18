#include "Game/MapItf.h"
#include "Game/Slice.h"
#include <cmath>

namespace Game
{

Engine::Coordinate MapItf::GroundCoord(Engine::Position pos) const
{
    return Game::Position(pos.x, pos.y, GroundHeight(Position(pos))).Coord();
}

Engine::Range<ZType> MapItf::Z() const
{
    auto bounds = GetBounds();
    return Engine::Range<ZType>(bounds.z);
}

ZType MapItf::GroundHeight(Position pos) const
{
    const auto& slice = SliceAt(pos.X(), pos.Y());
    auto hole = slice.FindBiggestNonSolidOpening();
    if (hole.IsEmpty())
        return pos.Z();
    else
        return hole.begin;
}

Slice MapItf::GetSlice(Position pos, ZType height) const
{
    const auto& slice = SliceAt(pos.X(), pos.Y());
    Engine::Range<ZType> heightRange(pos.Z(), pos.Z() + height);
    return slice & heightRange;
}

}

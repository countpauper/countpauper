#include "stdafx.h"
#include "VoxelMap.h"

namespace Game
{

VoxelMap::Data::Data() :
    Data(0, 0, 0)
{

}

VoxelMap::Data::Data(unsigned longitude, unsigned latitude, unsigned altitude) :
    longitude(longitude),
    latitude(latitude),
    altitude(altitude)
{
    voxels.resize(longitude * latitude * altitude);
}

unsigned VoxelMap::Data::Latitude() const
{
    return latitude;
}
unsigned VoxelMap::Data::Longitude() const
{
    return longitude;
}
unsigned VoxelMap::Data::Altitude() const
{
    return altitude;
}


unsigned VoxelMap::Data::Index(const Position& p) const
{
    const auto& stride = Stride();
    return p.x * stride.x + p.y * stride.y + p.z * stride.z;
}

const VoxelMap::Voxel& VoxelMap::Data::operator[](const Position& p) const
{
    return voxels.at(Index(p));
}

VoxelMap::Voxel& VoxelMap::Data::operator[](const Position& p)
{   // can't get outside
    return voxels.at(Index(p));
}

bool VoxelMap::Data::IsInside(const Position& p) const
{
    return p.x >= 0 &&
        p.y >= 0 &&
        p.z >= 0 &&
        unsigned(p.x) < longitude &&
        unsigned(p.y) < latitude &&
        unsigned(p.z) < altitude;
}

Position VoxelMap::Data::GetPosition(const Voxel* pVoxel) const
{
    auto index = pVoxel - voxels.data();
    if ((index >= 0) && (unsigned(index) < voxels.size()))
    {
        const auto& stride = Stride();
        int pz = (int(index) / stride.z) % altitude;
        int py = ((int(index) - (pz*stride.z)) / stride.y) % latitude;
        int px = ((int(index) - (pz*stride.z) - (py*stride.y)) / stride.x) % longitude;
        return Position(px, py, pz);
    }
    return Position();
}

Position VoxelMap::Data::Stride() const
{   // ordered Z,Y,X, in direction of iy for best caching
    return Position(altitude*latitude, altitude, 1);
}

}
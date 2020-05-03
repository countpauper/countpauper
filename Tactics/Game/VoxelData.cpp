#include "stdafx.h"
#include "VoxelMap.h"

namespace Game
{

VoxelMap::Data::iterator::iterator(const Data& data, const Position& position) :
    data(data),
    position(position)
{
}
VoxelMap::Data::iterator& VoxelMap::Data::iterator::operator++()
{
    if (++position.z == data.Altitude())
    {
        position.z = 0;
        if (++position.y == data.Latitude())
        {
            position.y = 0;
            if (++position.x == data.Longitude())
            {
                // end
            }
        }
    }
    return *this;
}


bool VoxelMap::Data::iterator::operator==(const iterator& other) const
{
    return &data == &other.data && position == other.position;
}

bool VoxelMap::Data::iterator::operator!=(const iterator& other) const
{
    return &data != &other.data || position != other.position;
}

VoxelMap::Data::iterator::value_type VoxelMap::Data::iterator::operator*() const
{
    return data[position];
}


VoxelMap::Data::Data() :
    Data(0, 0, 0)
{

}

VoxelMap::Data::Data(unsigned longitude, unsigned latitude, unsigned altitude) :
    longitude(longitude),
    latitude(latitude),
    altitude(altitude)
{
    size_t size = longitude * latitude * altitude;
    material.resize(size, &Material::vacuum);
    t.resize(size, 0);
    p.resize(size, 0);
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

Directions VoxelMap::Data::IsBoundary(const Position& p) const
{
    return VoxelMap::IsBoundary(p, Position(longitude, latitude, altitude));
}

unsigned VoxelMap::Data::Index(const Position& p) const
{
    const auto& stride = Stride();
    return p.x * stride.x + p.y * stride.y + p.z * stride.z;
}

VoxelMap::Voxel VoxelMap::Data::operator[](const Position& position) const
{   // can't get outside
    auto index = Index(position);
    return Voxel{ *material.at(index), t.at(index), p.at(index), position, IsBoundary(position) };
}


void VoxelMap::Data::Set(const Position& position, const Material& material, double temperature, double pressure)
{
    auto index = Index(position);
    this->material[index] = &material;
    t[index] = float(temperature);
    p[index] = float(material.Density(pressure, temperature));
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

Position VoxelMap::Data::Stride() const
{   // ordered Z,Y,X, in direction of iy for best caching
    return Position(altitude*latitude, altitude, 1);
}

}
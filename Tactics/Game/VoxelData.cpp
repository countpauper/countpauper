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
    size_t gridSize = (longitude+2) * (latitude+2) * (altitude+2);
    material.resize(gridSize, &Material::vacuum);
    t.resize(gridSize, 0);
    p.resize(gridSize, 0);
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
    Directions result;
    if (p.x == -1)
        result |= Direction::west;
    if (p.x == longitude)
        result |= Direction::east;
    if (p.y == -1)
        result |= Direction::south;
    if (p.y == latitude)
        result |= Direction::north;
    if (p.z == 0)
        result |= Direction::down;
    if (p.z == altitude)
        result |= Direction::up;
    return result;
}

unsigned VoxelMap::Data::Index(const Position& p) const
{
    const auto& stride = Stride();
    return (p.x+1) * stride.x + (p.y+1) * stride.y + (p.z+1) * stride.z;
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
    return Position((altitude+2)*(latitude+2), (altitude+2), 1);
}

}
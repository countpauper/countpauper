#include "stdafx.h"
#include "VoxelMap.h"

namespace Game
{

VoxelMap::Data::iterator::iterator(const Data& data, const Position& position) :
    iterator(data, position, Position(data.longitude, data.latitude, data.altitude))
{
}

VoxelMap::Data::iterator::iterator(const Data& data, const Position& position, const Position& end) :
    data(data),
    start(position),
    position(position),
    end(end)
{
}

VoxelMap::Data::iterator& VoxelMap::Data::iterator::operator++()
{
    if (++position.z == end.z)
    {
        if (++position.y == end.y)
        {
            if (++position.x == end.x)
            {
                // end
            }
            else
            {
                position.y = start.y;
                position.z = start.z;
            }
        }
        else
        {
            position.z = start.z;
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
    altitude(altitude),
    // allocate flux 1 extra size in all directions for boundary conditions on edge
    // allocate 2 extra boundary in all perpendicular directions for boundary conditions at edge +0.5
    u(Direction::east, longitude+1, latitude+2, altitude+2),
    v(Direction::north, longitude+2, latitude+1, altitude+2),
    w(Direction::up, longitude+2, latitude+2, altitude+1)
{
    // allocate grid with 2 boundary values on each size, for gradient at edge
    // Computed map state ranges from 0 to < long,lat,alt
    // Controlled boundary stat is at -1 and long lat alt
    size_t gridSize = (longitude+2) * (latitude+2) * (altitude+2);
    material.resize(gridSize, &Material::vacuum);
    temperature.resize(gridSize, 0);
    density.resize(gridSize, 0);
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

Engine::Vector VoxelMap::Data::Flow(const Position& p) const
{
    // Q = flux (kg/sm2) / density (kg/m3),     https://en.wikipedia.org/wiki/Volumetric_flow_rate
    Engine::Vector Q(
        0.5*(u[Position(p.x + 1, p.y, p.z)] + u[p]),
        0.5*(v[Position(p.x, p.y + 1, p.z)] + v[p]),
        0.5*(w[Position(p.x, p.y, p.z + 1)] + w[p]));
    auto gridDensity = density[GridIndex(p)];
    Q /= gridDensity;

    // v(m/s) = Q/A (Q = volume flow rate (m3/s), A = area(m2)) https://en.wikipedia.org/wiki/Flow_velocity
    constexpr double uSurface = HorizontalEl * VerticalEl * MeterPerEl * MeterPerEl;
    constexpr double vSurface = uSurface;
    constexpr double wSurface = HorizontalEl * HorizontalEl * MeterPerEl * MeterPerEl;
    Engine::Vector flow_velocity(
        Q.x / uSurface,
        Q.y / vSurface,
        Q.z / wSurface
    );
    return flow_velocity;
}



Engine::Vector VoxelMap::Data::FluxGradient(const Position& p) const
{
    constexpr double dX = HorizontalEl * MeterPerEl;
    constexpr double dY = HorizontalEl * MeterPerEl;
    constexpr double dZ = VerticalEl * MeterPerEl;

    return Engine::Vector(
        u.Gradient(p)/dX, 
        v.Gradient(p)/dY,
        w.Gradient(p)/dZ);
}

float VoxelMap::Data::Density(const Position& position) const
{
    return density[GridIndex(position)];
}

void VoxelMap::Data::SetDensity(const Position& position, float pressure)
{
    density[GridIndex(position)] = pressure;
}

Position VoxelMap::Data::Stride() const
{   // ordered Z,Y,X, in direction of iy for best caching
    return Position((altitude + 2)*(latitude + 2), (altitude + 2), 1);
}

unsigned VoxelMap::Data::GridIndex(const Position& p) const
{
    const auto& stride = Stride();
    return (p.x+1) * stride.x + (p.y+1) * stride.y + (p.z+1) * stride.z;
}

VoxelMap::Voxel VoxelMap::Data::operator[](const Position& position) const
{   // can't get outside
    auto index = GridIndex(position);
    assert(index >= 0);
    assert(index < density.size());
    return Voxel{ *material.at(index), temperature.at(index), density.at(index), position, IsBoundary(position) };
}

VoxelMap::Data::Boundary::Boundary(const Data& data, const Directions& directions) :
    data(data),
    directions(directions)
{
}
VoxelMap::Data::iterator VoxelMap::Data::Boundary::begin() const
{
    if (directions[Direction::none])
    {
        Position start(0, 0, 0);
        for (auto dir : directions)
        {
            // extend in direction
            // NB: corners are only added if overlapping directions
            if (dir.IsNegative())
            {   
                start += dir.Vector();
            }
        }
        return iterator(data, start, end().position);
    }
    else
    {
        Position start(data.longitude+1, data.latitude+1, data.altitude+1);

        for (auto dir : directions)
        {
            if (dir.IsNegative())
            {
                //                          if axis(==-1): -1 else  ... still -0 (ignore corner)
                start.x = std::min(start.x, dir.Vector().x);
                start.y = std::min(start.y, dir.Vector().y);
                start.z = std::min(start.z, dir.Vector().z);
            }
            else if (dir.IsPosititve())
            {                               // if axis(==1): long/lat/alt       else  0 (no corner)
                start.x = std::min(start.x, dir.Vector().x * int(data.longitude));
                start.y = std::min(start.y, dir.Vector().y * int(data.latitude) );
                start.z = std::min(start.z, dir.Vector().z * int(data.altitude) );
            }
        }
        return iterator(data, start, end().position);
    }

}
VoxelMap::Data::iterator VoxelMap::Data::Boundary::end() const
{
    if (directions[Direction::none])
    {
        Position stop(data.longitude, data.latitude, data.altitude);
        for (auto dir : directions)
        {
            // extend in direction
            // NB: corners are only added if overlapping directions
            if (dir.IsPosititve())
            {
                stop += dir.Vector();
            }
        }
        return iterator(data, stop, stop);
    }
    else
    {
        Position stop(-1, -1, -1);
        for (auto dir : directions)
        {
            if (dir.IsNegative())
            {  //           if axis(==-1): 0, else long/lat/alt, inside limit
                stop.x = std::max(stop.x, (1 + dir.Vector().x) * (int(data.longitude)));
                stop.y = std::max(stop.y, (1 + dir.Vector().y) * (int(data.latitude)));
                stop.z = std::max(stop.z, (1 + dir.Vector().z) * (int(data.altitude)));
            }
            else if (dir.IsPosititve())
            {   //          if axis(==1): grid limit = long/alt/lat+1, else inside limit
                stop.x = std::max(stop.x, dir.Vector().x + (int(data.longitude)));
                stop.y = std::max(stop.y, dir.Vector().y + (int(data.latitude)));
                stop.z = std::max(stop.z, dir.Vector().z + (int(data.altitude)));
            }
        }

        return iterator(data, stop, stop);
    }
}


void VoxelMap::Data::SetPressure(const Position& position, const Material& material, double temperature, double pressure)
{
    auto index = GridIndex(position);
    this->material[index] = &material;
    this->temperature[index] = float(temperature);
    this->density[index] = float(material.Density(pressure, temperature));
}

void VoxelMap::Data::AdjustGrid(const Position& position, double temperature, double density)
{
    auto index = GridIndex(position);
    this->temperature[index] = float(temperature);
    this->density[index] = float(density);
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

VoxelMap::Data::Flux::Flux(Direction direction, unsigned longitude, unsigned latitude, unsigned altitude) :
    direction(direction),
    offset(Position(-1,-1,-1) + direction.Vector()),
    longitude(longitude),
    latitude(latitude),
    altitude(altitude),
    flux(longitude*latitude*altitude,0)
{
    assert(direction.IsPosititve());    // for offset, vector should be positive
}

float VoxelMap::Data::Flux::operator[](const Position& p) const
{
    return flux.at(Index(p));
}

float& VoxelMap::Data::Flux::operator[](const Position& p)
{
    return flux.at(Index(p));
}

unsigned VoxelMap::Data::Flux::Index(const Position& p) const
{
    auto indexPosition = p - offset;
    return (indexPosition.x)*latitude*altitude+ indexPosition.y*altitude + indexPosition.z;

}

double VoxelMap::Data::Flux::Gradient(const Position& p) const
{
    return (*this)[p + direction.Vector()] - (*this)[p];
}

VoxelMap::Data::Flux::iterator VoxelMap::Data::Flux::begin() const
{
    return iterator(*this, Position(1, 1, 1) + offset);
}

VoxelMap::Data::Flux::iterator VoxelMap::Data::Flux::end() const
{
    return iterator(*this, Position(longitude-1, latitude-1, altitude-1)+offset);
}

VoxelMap::Data::Flux::iterator::iterator(const VoxelMap::Data::Flux& data, const Position& start) :
    iterator(data, start, Position(data.longitude-1, data.latitude-1, data.altitude-1)+data.offset)
{

}

VoxelMap::Data::Flux::iterator::iterator(const VoxelMap::Data::Flux& data, const Position& start, const Position& end) :
    data(data),
    start(start),
    position(start),
    end(end)
{
    // clip to end
    if ((position.x >= end.x) || 
        (position.y >= end.y) || 
        (position.z >= end.z))
    {
        position.z = end.z;
        position.y = end.y;
        position.x = end.x;
    }
}

VoxelMap::Data::Flux::iterator& VoxelMap::Data::Flux::iterator::operator++()
{
    if (++position.z == end.z)
    {
        if (++position.y == end.y)
        {
            if (++position.x == end.x)
            {
                // done
            }
            else
            {
                position.y = start.y;
                position.z = start.z;
            }
        }
        else
        {
            position.z = start.z;
        }
    }
    return *this;
}

bool VoxelMap::Data::Flux::iterator::operator==(const iterator& other) const
{
    return &data == &other.data &&
        position == other.position;
}

VoxelMap::Data::Flux::iterator::value_type VoxelMap::Data::Flux::iterator::operator*() const
{
    return std::make_pair(position, data[position]);
}

Directions VoxelMap::Data::Flux::iterator::IsBoundary() const
{
    return data.IsBoundary(position);
}

Directions VoxelMap::Data::Flux::IsBoundary(const Position& p) const
{
    Directions result;
    Position index = p + offset;
    assert(index.x >= 0 && index.x < int(longitude));
    assert(index.y >= 0 && index.y < int(latitude));
    assert(index.z >= 0 && index.z < int(altitude));
    if (index.x == 0)
        result |= Direction::west;
    if (index.x == longitude-1)
        result |= Direction::east;
    if (index.y == 0)
        result |= Direction::south;
    if (index.y == latitude-1)
        result |= Direction::north;
    if (index.z == 0)
        result |= Direction::down;
    if (index.y == altitude-1)
        result |= Direction::up;
    return result;
}

VoxelMap::Data::Flux::Boundary::Boundary(const Flux& data, const Direction& direction) :
    data(data),
    direction(direction)
{
}

VoxelMap::Data::Flux::iterator VoxelMap::Data::Flux::Boundary::begin() const
{
    if (direction.IsPosititve())
    {
        Position start(
            // if direction(==1) start 1 from end       else       start at (offset-offset)=0
            direction.Vector().x * (data.longitude - 1) +(direction.Vector().x-1) * data.offset.x,
            direction.Vector().y * (data.latitude - 1) + (direction.Vector().y-1) * data.offset.y,
            direction.Vector().z * (data.altitude - 1) + (direction.Vector().z-1) * data.offset.z);
        return Flux::iterator(data, start + data.offset, end().position);
    }
    else if (direction.IsNegative())
    {
        // Start at 0,0,0 index position always, either for border direction or perpendicular range
        Position start(0, 0, 0);
        return Flux::iterator(data, start + data.offset, end().position);
    }
    else
    {
        return data.begin();
    }
}

VoxelMap::Data::Flux::iterator VoxelMap::Data::Flux::Boundary::end() const
{
    if (direction.IsPosititve())
    {   // end at long/lat/alt index position always, either for border direction or perpendicular range
        Position stop(data.longitude, data.latitude, data.altitude) ;
        return Flux::iterator(data, stop + data.offset, stop + data.offset);
    }
    else if (direction.IsNegative())
    {
        Position stop(
            //  if direction(==-1): stop at 1+(offset-offset) index, else: stop at end of perpendicular index range
            direction.Vector().x * (data.offset.x-1) + (1 + direction.Vector().x) * data.longitude,
            direction.Vector().y * (data.offset.y-1) + (1 + direction.Vector().y) * data.latitude,
            direction.Vector().z * (data.offset.z-1) + (1 + direction.Vector().z) * data.altitude   );
        return Flux::iterator(data, stop + data.offset, stop + data.offset);
    }
    else
    {
        return data.end();
    }
}

}
#include "stdafx.h"
#include "VoxelMap.h"
#include "Engine/Maths.h"
#include "Engine/AxisAlignedBoundingBox.h"
#include "Engine/Utils.h"

namespace Game
{

VoxelMap::Data::iterator::iterator(const Data& data, const Position& position) :
    iterator(data, Box(position, data.size))
{
}

VoxelMap::Data::iterator::iterator(const Data& data, const Box& box) :
    data(data),
    box(box),
    position(box.Start())
{
    // clip to end
    if (!box.Contains(position))
    {
        position = box.End();
    }
}

VoxelMap::Data::iterator& VoxelMap::Data::iterator::operator++()
{
    if (++position.z >= box.z.end)
    {
        if (++position.y >= box.y.end)
        {
            if (++position.x >= box.x.end)
            {
                // end
            }
            else
            {
                position.y = box.y.begin;
                position.z = box.z.begin;
            }
        }
        else
        {
            position.z = box.z.begin;
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
    size(longitude, latitude, altitude),
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

unsigned VoxelMap::Data::Longitude() const
{
    return size.x;
}

unsigned VoxelMap::Data::Latitude() const
{
    return size.y;
}

unsigned VoxelMap::Data::Altitude() const
{
    return size.z;
}

Directions VoxelMap::Data::IsBoundary(const Position& p) const
{
    Directions result;
    if (p.x == -1)
        result |= Direction::west;
    if (p.x == size.x)
        result |= Direction::east;
    if (p.y == -1)
        result |= Direction::south;
    if (p.y == size.y)
        result |= Direction::north;
    if (p.z == -1)
        result |= Direction::down;
    if (p.z == size.z)
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

Engine::Vector VoxelMap::Data::DensityGradient(const Position& position) const
{
    return Engine::Vector(
        (Density(position + Position(1, 0, 0)) - Density(position + Position(-1, 0, 0))) / dX,
        (Density(position + Position(0, 1, 0)) - Density(position + Position( 0,-1, 0))) / dY,
        (Density(position + Position(0, 0, 1)) - Density(position + Position(0, 0, -1))) / dZ
    );
}

Engine::Vector VoxelMap::Data::FluxGradient(const Position& p) const
{
    return Engine::Vector(
        u.Gradient(p)/dX, 
        v.Gradient(p)/dY,
        w.Gradient(p)/dZ);
}

float VoxelMap::Data::Density(const Position& position) const
{
    return density[GridIndex(position)];
}

float VoxelMap::Data::Temperature(const Position& position) const
{
    return temperature[GridIndex(position)];
}

const VoxelMap::Material& VoxelMap::Data::MaterialAt(const Position& position) const
{
    return *material[GridIndex(position)];
}
double VoxelMap::Data::Density(const Engine::Coordinate& c) const
{
    auto gridPosition = Grid(c - Engine::Vector(0.5*dX, 0.5*dY, 0.5*dZ));
    double d[8];
    int i = 0;
    Position dP;
    for (dP.x = 0; dP.x <= 1; ++dP.x)
    {
        for (dP.y = 0; dP.y <= 1; ++dP.y)
        {
            for (dP.z = 0; dP.z <= 1; ++dP.z)
            {
                d[i++] = Density(gridPosition + dP);
            }
        }
    }
    Engine::Vector gridWeight = c - Center(gridPosition);
    gridWeight.x /= dX;
    gridWeight.y /= dY;
    gridWeight.z /= dZ;
    return TrilinearInterpolation(d, gridWeight);
}

double VoxelMap::Data::Temperature(const Engine::Coordinate& c) const
{
    auto gridPosition = Grid(c - Engine::Vector(0.5*dX, 0.5*dY, 0.5*dZ));
    double t[8];
    int i = 0;
    Position dP;
    for (dP.x = 0; dP.x <= 1; ++dP.x)
    {
        for (dP.y = 0; dP.y <= 1; ++dP.y)
        {
            for (dP.z = 0; dP.z <= 1; ++dP.z)
            {
                t[i++] = Temperature(gridPosition + dP);
            }
        }
    }
    Engine::Vector gridWeight = c - Center(gridPosition);
    gridWeight.x /= dX;
    gridWeight.y /= dY;
    gridWeight.z /= dZ;
    return TrilinearInterpolation(t, gridWeight);
}

void VoxelMap::Data::SetDensity(const Position& position, float pressure)
{
    density[GridIndex(position)] = pressure;
}

Position VoxelMap::Data::Stride() const
{   // ordered Z,Y,X, in direction of iy for best caching
    return Position((size.z + 2)*(size.y + 2), (size.z + 2), 1);
}

unsigned VoxelMap::Data::GridIndex(const Position& p) const
{
    if (!Bounds().Contains(p))
        throw std::out_of_range((std::string("Voxel Position'") + std::to_string(p) + "' out of range").c_str());
    return UncheckedGridIndex(p);
}

unsigned VoxelMap::Data::UncheckedGridIndex(const Position& p) const
{
    const auto& stride = Stride();
    return (p.x + 1) * stride.x + (p.y + 1) * stride.y + (p.z + 1) * stride.z;
}

VoxelMap::Voxel VoxelMap::Data::operator[](const Position& position) const
{   // can't get outside
    auto index = GridIndex(position);
    return Voxel{ *material.at(index), temperature.at(index), density.at(index), position, IsBoundary(position) };
}

VoxelMap::Data::Boundary::Boundary(const Data& data, const Direction& direction) :
    data(data),
    direction(direction)
{
}
VoxelMap::Data::iterator VoxelMap::Data::Boundary::begin() const
{
    if (direction.IsNone())
    {
        Position start(0, 0, 0);
        // extend in direction
        // NB: corners are only added if overlapping directions
        return iterator(data, Box(start, end().position));
    }
    else
    {
        Position start(data.size.x+1, data.size.y+1, data.size.z+1);

        if (direction.IsNegative())
        {
            start = direction.Vector();
        }
        else if (direction.IsPosititve())
        {    // if axis(==1): long/lat/alt       else  0 (no corner)
            start.x = direction.Vector().x * data.size.x;
            start.y = direction.Vector().y * data.size.y;
            start.z = direction.Vector().z * data.size.z;
        }
        return iterator(data, Box(start, end().position));
    }

}
VoxelMap::Data::iterator VoxelMap::Data::Boundary::end() const
{
    if (direction.IsNone())
    {
        Position stop(data.size);
        return iterator(data, Box(stop, stop));
    }
    else
    {
        Position stop;
        if (direction.IsNegative())
        {  //           if axis(==-1): 0, else long/lat/alt, inside limit
            stop.x = (1 + direction.Vector().x) * data.size.x;
            stop.y = (1 + direction.Vector().y) * data.size.y;
            stop.z = (1 + direction.Vector().z) * data.size.z;
        }
        else if (direction.IsPosititve())
        {   //          if axis(==1): grid limit = long/alt/lat+1, else inside limit
            stop = direction.Vector() + data.size;
        }
        return iterator(data, Box(stop, stop));
    }
}

VoxelMap::Data::iterator::difference_type operator-(const VoxelMap::Data::iterator& a, const VoxelMap::Data::iterator& b)
{
    return a.position - b.position;
}

VoxelMap::Data::Flux::iterator::difference_type operator-(const VoxelMap::Data::Flux::iterator& a, const VoxelMap::Data::Flux::iterator& b)
{
    return a.position - b.position;
}

VoxelMap::Data::Section::Section(const Data& data, const Engine::AABB& meters) :
    Section(data, Box(data.Clip(Grid(meters.Begin())), data.Clip(Grid(meters.End()))))
{
}

VoxelMap::Data::Section::Section(const Data& data, const Box& box) :
    data(data),
    box(box)
{
}

VoxelMap::Data::iterator VoxelMap::Data::Section::begin() const
{
    return iterator(data, box);
}

VoxelMap::Data::iterator VoxelMap::Data::Section::end() const
{
    return iterator(data, Box(box.End(), box.End()));
}

VoxelMap::Data::Section VoxelMap::Data::In(const Engine::AABB& meters) const
{
    return Section(*this, meters);
}

Box VoxelMap::Data::Bounds() const
{
    return Box(Position(0, 0, 0), size).Grow(1);
}

VoxelMap::Data::Section VoxelMap::Data::All() const
{
    return Section(*this, Bounds());
}

Position VoxelMap::Data::Clip(const Position& p) const
{
    return Bounds().Clip(p);
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
    return Bounds().Grow(-1).Contains(p);
}

VoxelMap::Data::Flux::Flux(Direction axis, unsigned longitude, unsigned latitude, unsigned altitude) :
    axis(axis),
    offset(Position(-1, -1, -1) + axis.Vector()),
    size(longitude, latitude, altitude),
    flux(size.Volume(), 0)
{
    assert(axis.IsPosititve());    // for offset, vector should be positive
    InitializeCorners();
}



Box VoxelMap::Data::Flux::Bounds() const
{
    return Box(offset, offset + size);
}

Box VoxelMap::Data::Flux::Edge(const Direction& dir) const
{
    Box b(Bounds());
    auto v = dir.Vector();
    if (dir.IsPosititve())
    {
        return Box(
            Position(
                b.x.begin + (1 - v.x),
                b.y.begin + (1 - v.y),
                b.z.begin + (1 - v.z)),
            Position(
                b.x.begin + v.x + (1 - v.x) * (size.x-1),
                b.y.begin + v.y + (1 - v.y) * (size.y-1),
                b.z.begin + v.z + (1 - v.z) * (size.z-1))
        );
    }
    else if (dir.IsNegative())
    {
        return Box(
            Position(   // TODO b.End()+v
                b.x.begin + (1+v.x) + (-v.x * (size.x-1)),
                b.y.begin + (1+v.y) + (-v.y * (size.y-1)),
                b.z.begin + (1+v.z) + (-v.z * (size.z-1))),
            Position(
                b.x.end - (1+v.x),
                b.y.end - (1+v.y),
                b.z.end - (1+v.z))
            );
    }
    else
    {
        return b.Grow(-1);
    }
}


void VoxelMap::Data::Flux::InitializeCorners()
{
    for (auto d1 : Direction::all)
    {
        for (auto d2 : Direction::all)
        {
            if (d1.IsParallel(d2))
                continue;
            Corner corner(*this, d1, d2);
            OutputDebugStringW((std::wstring(L"Corner ")+Engine::ToWString(d1) +L" / " + Engine::ToWString(d2) + L" Volume= " + std::to_wstring(corner.box.Volume())+L"\n").c_str());

            for (auto fluxPair : corner)
            {
                (*this)[fluxPair.first] = std::numeric_limits<float>::signaling_NaN();
            }
        }
    }
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
    if (!Bounds().Contains(p))
        throw std::out_of_range((std::string("Flux position '") + Engine::ToString(p) + "' out of range").c_str());

    auto indexPosition = p - offset;
    return (indexPosition.x)*size.y*size.z+ indexPosition.y*size.z + indexPosition.z;

}

bool VoxelMap::Data::Flux::IsOffset(const Direction& d) const
{
    return !axis.IsOpposite(d);
}

void VoxelMap::Data::Flux::SetBoundary(const Position& p, const Direction& dir, double boundaryFlux)
{
    if (dir.IsParallel(axis))
    {
        (*this)[p] = float(boundaryFlux);
    }
    else
    {
        (*this)[p] = Extrapolate(p, dir, boundaryFlux);
    }
}

double VoxelMap::Data::Flux::Gradient(const Position& p) const
{
    return (*this)[p + axis.Vector()] - (*this)[p];
}

float VoxelMap::Data::Flux::Extrapolate(const Position& outsidePosition, const Direction& dir, double  boundaryFlux) const
{
    Position insidePosition = outsidePosition - dir.Vector();
    auto insideFlux = (*this)[insidePosition];
    auto outsideFlux = Engine::Lerp(insideFlux, float(boundaryFlux), 2.0);
    return outsideFlux;
}


VoxelMap::Data::Flux::iterator VoxelMap::Data::Flux::begin() const
{
    return iterator(*this, Position(1, 1, 1) + offset);
}

VoxelMap::Data::Flux::iterator VoxelMap::Data::Flux::end() const
{
    return iterator(*this, Position(size.x-1, size.y-1, size.z-1)+offset);
}

VoxelMap::Data::Flux::iterator::iterator(const VoxelMap::Data::Flux& data, const Position& start) :
    iterator(data, Box(start, Position(data.size.x-1, data.size.y-1, data.size.z-1)+data.offset))
{
}

VoxelMap::Data::Flux::iterator::iterator(const VoxelMap::Data::Flux& data, const Box& box) :
    data(data),
    box(box),
    position(box.Start())
{
    if (!box.Contains(position))
    {    // clip to end
        position = box.End();
    }
}

VoxelMap::Data::Flux::iterator& VoxelMap::Data::Flux::iterator::operator++()
{
    if (++position.z >= box.z.end)
    {
        if (++position.y >= box.y.end)
        {
            if (++position.x >= box.x.end)
            {
                // done
            }
            else
            {
                position.y = box.y.begin;
                position.z = box.z.begin;
            }
        }
        else
        {
            position.z = box.z.begin;
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
    assert(index.x >= 0 && index.x < size.x);
    assert(index.y >= 0 && index.y < size.y);
    assert(index.z >= 0 && index.z < size.z);
    if (index.x == 0)
        result |= Direction::west;
    if (index.x == size.x - 1)
        result |= Direction::east;
    if (index.y == 0)
        result |= Direction::south;
    if (index.y == size.y - 1)
        result |= Direction::north;
    if (index.z == 0)
        result |= Direction::down;
    if (index.y == size.z - 1)
        result |= Direction::up;
    return result;
}

VoxelMap::Data::Flux::Section::Section(const Flux& data, const Box& box) :
    data(data),
    box(box)
{
}

VoxelMap::Data::Flux::iterator VoxelMap::Data::Flux::Section::begin() const
{
    return Flux::iterator(data, box);
}

VoxelMap::Data::Flux::iterator VoxelMap::Data::Flux::Section::end() const
{
    return Flux::iterator(data, Box(box.End(), box.End()));
}

VoxelMap::Data::Flux::Boundary::Boundary(const Flux& data, const Direction& direction) :
    Section(data, data.Edge(direction))
{
}

/*
Box VoxelMap::Data::Flux::Boundary::Bounds(const Flux& data, const Direction& direction)
{
    if (direction.IsPosititve())
    {
        return Box(
            Position(
                // if direction(==1) start 1 from end       else       start at (offset-offset)=0
                direction.Vector().x * (data.size.x - 1) + (direction.Vector().x - 1) * data.offset.x,
                direction.Vector().y * (data.size.y - 1) + (direction.Vector().y - 1) * data.offset.y,
                direction.Vector().z * (data.size.z - 1) + (direction.Vector().z - 1) * data.offset.z),
            data.offset + data.size);
    }
    else if (direction.IsNegative())
    {
        // Start at 1 index position, except start at 0 for direction(==-1)
        return Box(
            Position(1, 1, 1) + data.offset + direction.Vector(),
            data.offset + Position(
                //  if direction(==-1): stop at 1+(offset-offset) index, else: stop at end of perpendicular index range
                direction.Vector().x * (data.offset.x - 1) + (1 + direction.Vector().x) * data.size.x,
                direction.Vector().y * (data.offset.y - 1) + (1 + direction.Vector().y) * data.size.y,
                direction.Vector().z * (data.offset.z - 1) + (1 + direction.Vector().z) * data.size.z));
    }
    else
    {
        return data.Bounds();
    }
}

Position VoxelMap::Data::Flux::Boundary::To(const Flux& data, const Direction& direction)
{
    if (direction.IsPosititve())
    {   // end at long/lat/alt index position always, either for border direction or perpendicular range
    }
    else if (direction.IsNegative())
    {
        return ;
    }
    else
    {
        return data.end().position;
    }
}
*/

VoxelMap::Data::Flux::Corner::Corner(const Flux& data, const Direction& directionA, const Direction& directionB) :
    Section(data, (data.Edge(directionA) + Size(directionB.Axis().Vector() + directionA.Perpendicular(directionB).Vector()) & 
        (data.Edge(directionB)+Size(directionA.Axis().Vector() + directionA.Perpendicular(directionB).Vector()))))
{
}

/*
Position VoxelMap::Data::Flux::Corner::From(const Flux& data, const Direction& directionA, const Direction& directionB)
{
    return (Edge(directionA) & Edge(direcionB)).Start();

    if (direction.IsPosititve())
    {
        return Position(
            // if direction(==1) start 1 from end       else       start at (offset-offset)=0
            direction.Vector().x * (data.longitude - 1) + (direction.Vector().x - 1) * data.offset.x,
            direction.Vector().y * (data.latitude - 1) + (direction.Vector().y - 1) * data.offset.y,
            direction.Vector().z * (data.altitude - 1) + (direction.Vector().z - 1) * data.offset.z);
    }
    else if (direction.IsNegative())
    {
        // Start at 1 index position, except start at 0 for direction(==-1)
        return Position(1, 1, 1) + data.offset + direction.Vector();
    }
    else
    {
        return data.begin().position;
    }
}

Position VoxelMap::Data::Flux::Corner::To(const Flux& data, const Direction& directionA, const Direction& directionB)
{
    if (direction.IsPosititve())
    {   // end at long/lat/alt index position always, either for border direction or perpendicular range
        return data.offset + Position(data.longitude, data.latitude, data.altitude);
    }
    else if (direction.IsNegative())
    {
        return data.offset + Position(
            //  if direction(==-1): stop at 1+(offset-offset) index, else: stop at end of perpendicular index range
            direction.Vector().x * (data.offset.x - 1) + (1 + direction.Vector().x) * data.longitude,
            direction.Vector().y * (data.offset.y - 1) + (1 + direction.Vector().y) * data.latitude,
            direction.Vector().z * (data.offset.z - 1) + (1 + direction.Vector().z) * data.altitude);
    }
    else
    {
        return data.end().position;
    }
}
*/


}
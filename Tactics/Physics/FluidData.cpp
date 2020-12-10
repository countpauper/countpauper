#include "stdafx.h"
#include "FluidDynamics.h" 
#include "Engine/Maths.h"
#include "Engine/AxisAlignedBoundingBox.h"
#include "Engine/Utils.h"
#include <numeric>

namespace Physics
{

FluidDynamics::Data::Data(const Size& size, const Engine::Vector& grid) :
    VoxelData(size, 1, grid),
    // allocate flux 1 extra size in all directions for boundary conditions on edge
    // allocate 2 extra boundary in all perpendicular directions for boundary conditions at edge +0.5
    u(Direction::x, size+Size(1,2,2)),
    v(Direction::y, size+Size(2,1,2)),
    w(Direction::z, size+Size(2,2,1))
{
}

Engine::Vector FluidDynamics::Data::Flow(const Position& p) const
{
    // Q = flux (kg/sm2) / density (kg/m3),     https://en.wikipedia.org/wiki/Volumetric_flow_rate
    Engine::Vector Q(
        0.5*(u[Position(p.x - 1, p.y, p.z)] + u[p]),
        0.5*(v[Position(p.x, p.y - 1, p.z)] + v[p]),
        0.5*(w[Position(p.x, p.y, p.z - 1)] + w[p]));
    auto gridDensity = voxels[GridIndex(p)].density;
    Q /= gridDensity;

    // v(m/s) = Q/A (Q = volume flow rate (m3/s), A = area(m2)) https://en.wikipedia.org/wiki/Flow_velocity
    const double uSurface = grid.x*grid.z;
    const double vSurface = grid.y*grid.z;
    const double wSurface = grid.x*grid.y;
    Engine::Vector flow_velocity(
        Q.x / uSurface,
        Q.y / vSurface,
        Q.z / wSurface
    );
    return flow_velocity;
}

Engine::Vector FluidDynamics::Data::DensityGradient(const Position& position) const
{
    return Engine::Vector(
        (VoxelData::Density(position + Position(1, 0, 0)) - VoxelData::Density(position + Position(-1, 0, 0))) / grid.x,
        (VoxelData::Density(position + Position(0, 1, 0)) - VoxelData::Density(position + Position(0, -1, 0))) / grid.y,
        (VoxelData::Density(position + Position(0, 0, 1)) - VoxelData::Density(position + Position(0, 0, -1))) / grid.z
    );
}

Engine::Vector FluidDynamics::Data::FluxGradient(const Position& p) const
{
    return Engine::Vector(
        u.Gradient(p) / grid.x,
        v.Gradient(p) / grid.y,
        w.Gradient(p) / grid.z);
}



void FluidDynamics::Data::InvalidateCorners()
{
    for (auto d0 : Direction::all)
    {
        for (auto d1 : Direction::all)
        {
            if (d0.IsParallel(d1))
                continue;   // not a corner
            Corner corner(*this, d0 | d1);
            //OutputDebugStringW((std::wstring(L"Voxel Corner ") + Engine::ToWString(d0) + L" / " + Engine::ToWString(d1) + L" Volume= " + std::to_wstring(corner.box.Volume()) + L"\n").c_str());
            for (auto v : corner)
            {
                SetInvalid(v.first);
            }

        }
    }
}

FluidDynamics::Data::Flux::iterator::difference_type operator-(const FluidDynamics::Data::Flux::iterator& a, const FluidDynamics::Data::Flux::iterator& b)
{
    return a.position - b.position;
}


FluidDynamics::Data::Flux::Flux(Direction axis, const Size& size) :
    axis(axis),
    offset(-1, -1, -1),
    size(size),
    flux(size.Volume(), 0)
{
    assert(axis.IsPosititve());    // for offset, vector should be positive
}

Box FluidDynamics::Data::Flux::Bounds() const
{
    return Box(offset, offset + size);
}

Box FluidDynamics::Data::Flux::Edge(const Direction& dir) const
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
                b.x.begin + v.x + (1 - v.x) * (size.x - 1),
                b.y.begin + v.y + (1 - v.y) * (size.y - 1),
                b.z.begin + v.z + (1 - v.z) * (size.z - 1))
        );
    }
    else if (dir.IsNegative())
    {
        return Box(
            Position(
                b.x.begin + (1 + v.x) + (-v.x * (size.x - 1)),
                b.y.begin + (1 + v.y) + (-v.y * (size.y - 1)),
                b.z.begin + (1 + v.z) + (-v.z * (size.z - 1))),
            Position(
                b.x.end - (1 + v.x),
                b.y.end - (1 + v.y),
                b.z.end - (1 + v.z))
        );
    }
    else
    {
        return b.Grow(-1);
    }
}

Box FluidDynamics::Data::Flux::ExtendedEdge(const Direction& dir) const
{
    Box edge = Edge(dir);
    for (auto axis : Directions::axes)
        if (!dir.IsParallel(axis))
            edge.Grow(Size(axis.Vector()));
    return edge;
}

void FluidDynamics::Data::Flux::InvalidateCorners()
{
    for (auto d0 : Direction::all)
    {
        for (auto d1 : Direction::all)
        {
            if (d0.IsParallel(d1))
                continue;
            Corner corner(*this, d0 | d1);
            //OutputDebugStringW((std::wstring(L"Flux Corner ")+Engine::ToWString(d0) +L" / " + Engine::ToWString(d1) + L" Volume= " + std::to_wstring(corner.box.Volume())+L"\n").c_str());

            for (auto fluxPair : corner)
            {
                (*this)[fluxPair.first] = std::numeric_limits<float>::signaling_NaN();
            }
        }
    }
}

float FluidDynamics::Data::Flux::operator[](const Position& p) const
{
    return flux.at(Index(p));
}

float& FluidDynamics::Data::Flux::operator[](const Position& p)
{
    return flux.at(Index(p));
}

unsigned FluidDynamics::Data::Flux::Index(const Position& p) const
{
    if (!Bounds().Contains(p))
        throw std::out_of_range((std::string("Flux position '") + Engine::ToString(p) + "' out of range " + Engine::ToString(Bounds())).c_str());

    auto indexPosition = p - offset;
    return (indexPosition.x)*size.y*size.z + indexPosition.y*size.z + indexPosition.z;

}

bool FluidDynamics::Data::Flux::IsOffset(const Direction& d) const
{
    return !axis.IsOpposite(d);
}

void FluidDynamics::Data::Flux::SetBoundary(const Position& p, const Directions& boundary, double boundaryFlux)
{
    for (auto dir : boundary)
    {
        (*this)[p] = Extrapolate(p, boundary, boundaryFlux);
    }
}

double FluidDynamics::Data::Flux::Gradient(const Position& p) const
{
    return (*this)[p] - (*this)[p - axis.Vector()];
}

float FluidDynamics::Data::Flux::Extrapolate(const Position& outsidePosition, const Directions& boundary, double  boundaryFlux) const
{
    Position insideOffset(0, 0, 0);
    for (auto direction : boundary)
    {
        if (!direction.IsParallel(axis))
        {
            insideOffset -= direction.Vector();
        }
    }
    // TODO: trilinear extrapolation ... or bi or linear depending on boundary.size()
    Position insidePosition = outsidePosition - insideOffset;
    auto insideFlux = (*this)[insidePosition];
    auto outsideFlux = Engine::Lerp(insideFlux, float(boundaryFlux), 2.0);
    return outsideFlux;
}


FluidDynamics::Data::Flux::iterator FluidDynamics::Data::Flux::begin() const
{
    return iterator(*this, Position(1, 1, 1) + offset);
}

FluidDynamics::Data::Flux::iterator FluidDynamics::Data::Flux::end() const
{
    return iterator(*this, Position(size.x - 1, size.y - 1, size.z - 1) + offset);
}

FluidDynamics::Data::Flux::iterator::iterator(const FluidDynamics::Data::Flux& data, const Position& start) :
    iterator(data, Box(start, Position(data.size.x - 1, data.size.y - 1, data.size.z - 1) + data.offset))
{
}

FluidDynamics::Data::Flux::iterator::iterator(const FluidDynamics::Data::Flux& data, const Box& box) :
    data(data),
    box(box),
    position(box.Start())
{
    if (!box.Contains(position))
    {    // clip to end
        position = box.End();
    }
}

FluidDynamics::Data::Flux::iterator& FluidDynamics::Data::Flux::iterator::operator++()
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

bool FluidDynamics::Data::Flux::iterator::operator==(const iterator& other) const
{
    return &data == &other.data &&
        position == other.position;
}

FluidDynamics::Data::Flux::iterator::value_type FluidDynamics::Data::Flux::iterator::operator*() const
{
    return std::make_pair(position, data[position]);
}

Directions FluidDynamics::Data::Flux::iterator::IsBoundary() const
{
    return data.IsBoundary(position);
}

Directions FluidDynamics::Data::Flux::IsBoundary(const Position& p) const
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

FluidDynamics::Data::Flux::Section::Section(const Flux& data, const Box& box) :
    data(data),
    box(box)
{
}

FluidDynamics::Data::Flux::iterator FluidDynamics::Data::Flux::Section::begin() const
{
    return Flux::iterator(data, box);
}

FluidDynamics::Data::Flux::iterator FluidDynamics::Data::Flux::Section::end() const
{
    return Flux::iterator(data, Box(box.End(), box.End()));
}

FluidDynamics::Data::Flux::Boundary::Boundary(const Flux& data, const Direction& direction) :
    Section(data, data.Edge(direction))
{
}

FluidDynamics::Data::Flux::Corner::Corner(const Flux& data, const Directions& directions) :
    Section(data,
        std::accumulate(directions.begin(), directions.end(), data.Bounds(), [&data](const Box& bounds, const Direction& direction)
{
    return bounds & data.ExtendedEdge(direction);
}))
{
}


FluidDynamics::Data::Flux::Section FluidDynamics::Data::Flux::BoundaryCondition(const Direction& dir) const
{
    return Boundary(*this, dir);
}

FluidDynamics::Data::Flux::Section FluidDynamics::Data::Flux::BoundaryCondition(const Directions& boundary) const
{
    if (boundary.size() == 1)
        return Boundary(*this, *boundary.begin());
    else
        return Corner(*this, boundary);
}


}

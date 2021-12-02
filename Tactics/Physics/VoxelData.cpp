#include "stdafx.h"
#include "VoxelData.h"
#include <numeric>
#include "Engine/AxisAlignedBoundingBox.h"

namespace Physics
{

VoxelData::VoxelData() :
    VoxelData(Size(0))
{
}

VoxelData::VoxelData(const Size& size, int edge, const Engine::Vector& grid) :
    size(size),
    edge(edge),
    grid(grid)
{
    voxels.resize((size+edge+edge).Volume(), { &Material::vacuum, 0,0 });
}

double VoxelData::Density(const Position& position) const
{
    return static_cast<double>(voxels[GridIndex(position)].density);
}

double VoxelData::Density(const Engine::Coordinate& c) const
{
    auto gridPosition = grid(c - grid * 0.5);
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
    gridWeight.x /= grid.x;
    gridWeight.y /= grid.y;
    gridWeight.z /= grid.z;
    return TrilinearInterpolation(d, gridWeight);
}

double VoxelData::Density(const Engine::IVolume& area) const
{
    auto bb = area.GetBoundingBox();
    if (bb.Volume() == 0)
    {   // point
        return Density(bb.Begin());
    }
    else
    {
        bb &= BoundingBox();
        assert(false); // unimplemented
        return std::numeric_limits<float>::signaling_NaN();
    }
}

double VoxelData::Temperature(const Position& position) const
{
    return voxels[GridIndex(position)].temperature;
}

double VoxelData::Temperature(const Engine::Coordinate& c) const
{
    auto gridPosition = grid(c - grid * 0.5);
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
    Engine::Vector gridWeight = c - grid.Center(gridPosition);
    gridWeight.x /= grid.x;
    gridWeight.y /= grid.y;
    gridWeight.z /= grid.z;
    return TrilinearInterpolation(t, gridWeight);
}


double VoxelData::Temperature(const Engine::IVolume& area) const
{
    auto bb = area.GetBoundingBox();
    if (bb.Volume() == 0)
    {   // point
        return Temperature(bb.Begin());
    }
    else
    {
        bb &= BoundingBox();
        assert(false); // unimplemented
        return std::numeric_limits<float>::signaling_NaN();
    }
}


Engine::Vector VoxelData::Force(const Engine::IVolume& volume) const
{
    assert(false);
    return Engine::Vector();
}

const Material& VoxelData::MaterialAt(const Position& position) const
{
    return *(voxels[GridIndex(position)].material);
}

void VoxelData::SetDensity(const Position& position, float density)
{
    auto& v = voxels[GridIndex(position)];
    v.density = density;
}

Position VoxelData::Stride() const
{   // ordered Z,Y,X, in direction of iy for best caching
    return Position((size.z + edge.x*2)*(size.y + edge.y*2), (size.z + edge.z*2), 1);
}

unsigned VoxelData::GridIndex(const Position& p) const
{
    if (!Bounds().Contains(p))
        throw std::out_of_range((std::string("Voxel Position'") + std::to_string(p) + "' out of range").c_str());
    return UncheckedGridIndex(p);
}

Position VoxelData::ClippedGrid(const Engine::Coordinate& meters) const
{
    Box bounds = Bounds();
    Engine::AABB box(Engine::Coordinate(bounds.Start().x*grid.x, bounds.Start().y*grid.y, bounds.Start().z*grid.z),
        Engine::Coordinate(bounds.End().x*grid.x, bounds.End().y*grid.y, bounds.End().z*grid.z));
    return grid(box.Clip(meters));
}



Engine::Vector VoxelData::GridSize() const
{
    return grid;
}

double VoxelData::VoxelVolume() const
{
    return 1000.0 * grid.Volume();
}

Engine::Coordinate VoxelData::Center(const Position& p) const
{
    return Engine::Coordinate(
        (0.5 + p.x) * grid.x,
        (0.5 + p.y) * grid.y,
        (0.5 + p.z) * grid.z
    );
}

unsigned VoxelData::UncheckedGridIndex(const Position& p) const
{
    const auto& stride = Stride();
    return (p.x + edge.x) * stride.x + (p.y + edge.y) * stride.y + (p.z + edge.z) * stride.z;
}

const Voxel& VoxelData::operator[](const Position& position) const
{ 
    auto index = GridIndex(position);
    return voxels.at(index);
}

Size VoxelData::GetSize() const
{
    return size;
}

Voxel& VoxelData::operator[](const Position& position)
{  
    auto index = GridIndex(position);
    return voxels.at(index);
}


VoxelData::Section::Section(const VoxelData& data, const Box& box) :
    data(data),
    box(box)
{
}

VoxelData::const_iterator VoxelData::Section::begin() const
{
    return const_iterator(data, box, box.Start());
}

VoxelData::const_iterator VoxelData::Section::end() const
{
    return const_iterator(data, box, box.End());
}

VoxelData::Boundary::Boundary(const VoxelData& data, const Direction& direction) :
    Section(data, data.Edge(direction)),
    direction(direction)
{
}

Directions VoxelData::IsBoundary(const Position& p) const
{
    Directions result;
    if (p.x < 0)
        result |= Direction::west;
    if (p.x >= size.x)
        result |= Direction::east;
    if (p.y < 0)
        result |= Direction::south;
    if (p.y >= size.y)
        result |= Direction::north;
    if (p.z < 0)
        result |= Direction::down;
    if (p.z >= size.z)
        result |= Direction::up;
    return result;
}

VoxelData::Corner::Corner(const VoxelData& data, const Directions& directions) :
    Section(data,
        std::accumulate(directions.begin(), directions.end(), data.Bounds(), [data](const Box& box, Direction direction)
        {
            return box & data.ExtendedEdge(direction);
        }))
{
}

Box VoxelData::Insides() const
{
    return Box(Position(0, 0, 0), size);
}

Box VoxelData::Bounds() const
{
    return Insides().Grow(edge);
}

Engine::AABB VoxelData::BoundingBox() const
{
    auto box = Bounds();
    return Engine::AABB(
        Engine::Range<double>(box.x.begin, box.x.end) * grid.x,
        Engine::Range<double>(box.y.begin, box.y.end) * grid.y,
        Engine::Range<double>(box.z.begin, box.z.end) * grid.z
    );
}


Box VoxelData::Edge(const Direction& direction) const
{
    if (direction.IsNone())
    {
        return Insides();
    }
    else
    {
        Size cornerGrowth(0, 0, 0);
        if (direction.IsZ())
            cornerGrowth = Size(1, 1, 0);
        else if (direction.IsY())
            cornerGrowth = Size(1, 0, 0);

        Position v = direction.Vector();
        if (direction.IsNegative())
        {
            return Box(
                v,
                Position(
                    (1 + v.x) * size.x,
                    (1 + v.y) * size.y,
                    (1 + v.z) * size.z)
            ).Grow(cornerGrowth);
        }
        else
        {    // if axis(==1): long/lat/alt       else  0 (no corner)
            assert(direction.IsPosititve());
            return Box(
                Position(
                    v.x * size.x,
                    v.y * size.y,
                    v.z * size.z),
                Position(size.x, size.y, size.z) + v).Grow(cornerGrowth);
        }
    }
}

Box VoxelData::ExtendedEdge(const Direction& direction) const
{
    Box edge = Edge(direction);
    for (auto axis : Directions::axes)
    {
        if (!axis.IsParallel(direction))    // double negative for dirtecion::none
            edge.Grow(axis.Vector());
    }
    return edge;
}

VoxelData::Section VoxelData::All() const
{
    return Section(*this, Bounds());
}

VoxelData::Section VoxelData::In(const Box& box) const
{
    return Section(*this, box & Bounds());
}

VoxelData::Section VoxelData::In(const Engine::AABB& meters) const
{
    return In(Box(grid(meters.Begin()), grid(meters.End())));
}

Position VoxelData::Clip(const Position& p) const
{
    return Bounds().Clip(p);
}

size_t VoxelData::Fill(const Engine::IVolume& v, const Material& m, double density, const double temperature)
{
    size_t filled = 0;
    auto bb = v.GetBoundingBox() & BoundingBox();
    Box volBB(grid(bb.Begin()), grid(bb.End()) + Position(1, 1, 1));
    std::map<Position, Engine::Range<int>> dbg;
    for (auto& voxel : In(volBB))
    {
        auto center = Center(voxel.first);
        if (v.Contains(center))
        {
            (*this)[voxel.first] = { &m, float(temperature), float(density) };
            ++filled;
        }
    }
    return filled;
}


void VoxelData::SetInvalid(const Position& position)
{
    auto index = GridIndex(position);
    voxels[index] = Voxel{ nullptr, std::numeric_limits<float>::signaling_NaN(),  std::numeric_limits<float>::signaling_NaN() };
}

void VoxelData::SetPressure(const Position& position, const Material& material, double temperature, double pressure)
{
    auto index = GridIndex(position);
    voxels[index] = Voxel{ &material, float(temperature), float(material.Density(pressure, temperature)) };
}

void VoxelData::AdjustGrid(const Position& position, double temperature, double density)
{
    auto index = GridIndex(position);
    auto& v = voxels[index];
    v.temperature = float(temperature);
    v.density = float(density);
}

bool VoxelData::IsInside(const Position& p) const
{
    return Insides().Contains(p);
}

VoxelData::const_iterator::difference_type operator-(const VoxelData::const_iterator& a, const VoxelData::const_iterator& b)
{
    return a.position - b.position;
}

}
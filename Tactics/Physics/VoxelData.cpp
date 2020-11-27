#include "stdafx.h"
#include "VoxelData.h"
#include <numeric>
#include "Engine/AxisAlignedBoundingBox.h"

namespace Game
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

float VoxelData::Density(const Position& position) const
{
    return voxels[GridIndex(position)].density;
}

float VoxelData::Temperature(const Position& position) const
{
    return voxels[GridIndex(position)].temperature;
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


Position VoxelData::Grid(const Engine::Coordinate& meters) const
{
    return Position(
        int(std::floor(meters.x / grid.x)),
        int(std::floor(meters.y / grid.y)),
        int(std::floor(meters.z / grid.z)));
}

Position VoxelData::ClippedGrid(const Engine::Coordinate& meters) const
{
    Box bounds = Bounds();
    Engine::AABB box(Engine::Coordinate(bounds.Start().x*grid.x, bounds.Start().y*grid.y, bounds.Start().z*grid.z),
        Engine::Coordinate(bounds.End().x*grid.x, bounds.End().y*grid.y, bounds.End().z*grid.z));
    return Grid(box.Clip(meters));
}



Engine::Vector VoxelData::GridSize() const
{
    return grid;
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
    return In(Box(Grid(meters.Begin()), Grid(meters.End())));
}

Position VoxelData::Clip(const Position& p) const
{
    return Bounds().Clip(p);
}

size_t VoxelData::Fill(const Engine::IVolume& v, const Material& m, const double temperature)
{
    size_t filled = 0;
    auto bb = v.GetBoundingBox();
    Box volBB(ClippedGrid(bb.Begin()), ClippedGrid(bb.End()) + Position(1, 1, 1));

    for (auto& voxel : In(volBB))
    {
        auto center = Center(voxel.first);
        if (v.Distance(center) <= 0)
        {
            (*this)[voxel.first] = { &m, float(temperature), float(m.Density(PascalPerAtmosphere, temperature)) };
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
#pragma once
#include <string>
#include "Voxel.h"
#include "Position.h"
#include "Direction.h"
#include "Box.h"
#include "Engine/Vector.h"
#include "Engine/Volume.h"
#include "Grid.h"
#include "BoxIterator.h"

namespace Engine { struct AABB;  }
namespace Physics
{

class VoxelData
{
public:
    VoxelData();
    VoxelData(const Size& size, int border = 0, const Engine::Vector& grid=Engine::Vector(1,1,1));
    VoxelData(const VoxelData&) = default;
    const Voxel& operator[](const Position& p) const;
    Voxel& operator[](const Position& p);
    Size GetSize() const;
    Grid grid;
    Position ClippedGrid(const Engine::Coordinate& meters) const;
    Engine::Vector GridSize() const;
    double VoxelVolume() const; // liter

    Engine::Coordinate Center(const Position& p) const;
protected:

public:
    using iterator = BoxDataIterator<VoxelData, Voxel>;
    class const_iterator : public BoxDataIterator<const VoxelData, const Voxel>
    {
    public:
        explicit const_iterator(const VoxelData& data) : BoxDataIterator<const VoxelData, const Voxel>(data) {}
        explicit const_iterator(const VoxelData& data, const Box& box, const Position& position) : BoxDataIterator<const VoxelData, const Voxel>(data, box, position) {}
        const_iterator(const iterator& o) : const_iterator(o.data, o.box, o.position) {}
    };
    using value_type = const_iterator::value_type;
    // iterators
    const_iterator begin() const { return const_iterator(*this); }
    const_iterator end() const { return const_iterator(*this, Insides(), Insides().End()); }
    iterator begin() { return iterator(*this); }
    iterator end() { return iterator(*this, Insides(), Insides().End()); }

    class Section
    {
    public:
        Section(const VoxelData& data, const Box& box);

        const_iterator begin() const;
        const_iterator end() const;
        const VoxelData& data;
        const Box box;
    };
    class Boundary : public Section
    {
    public:
        Boundary(const VoxelData& data, const Direction& direction);
        Direction direction;
    };
    class Corner : public Section
    {
    public:
        Corner(const VoxelData& data, const Directions& directions);
    };
    Boundary BoundaryCondition(const Direction& dir) const { return Boundary(*this, dir); }
    Directions IsBoundary(const Position& position) const;

    Box Edge(const Direction& direction) const;
    Box ExtendedEdge(const Direction& direction) const;

    // Geometry getters
    bool IsInside(const Position& p) const;
    Section All() const;
    Section In(const Box& box) const;
    Section In(const Engine::AABB& meters) const;
    Box Insides() const;
    Box Bounds() const;
    Engine::AABB BoundingBox() const;

    Position Clip(const Position& p) const;

    // Data getters and setters
    size_t Fill(const Engine::IVolume& v, const Material& m, double temperature);   // volume in SI
    void SetPressure(const Position& location, const Material& material, double temperature, double pressure);
    void AdjustGrid(const Position& location, double temperature, double density);
    double Density(const Position& position) const;
    double Density(const Engine::Coordinate& coordinate) const;
    double Density(const Engine::IVolume& volume) const;
    double Temperature(const Position& position) const;
    double Temperature(const Engine::Coordinate& coordinate) const; // coordinate in SI
    double Temperature(const Engine::IVolume& volume) const;    // volume in SI
    Engine::Vector Force(const Engine::IVolume& volume) const;  // volume in SI
    const Material& MaterialAt(const Position& position) const;
    void SetDensity(const Position& p, float density);
protected:
    Position Stride() const;
    unsigned GridIndex(const Position& p) const;
    unsigned UncheckedGridIndex(const Position& p) const;
    void SetInvalid(const Position& position); 
protected:
    Size size;                  // valid grids from 0 to size
    Size edge;                  // edge grids in all Directions <0 or >=size
    std::vector<Voxel> voxels;  // longitude+2, latitude+2, altitude+2
};


VoxelData::const_iterator::difference_type operator-(const VoxelData::const_iterator& a, const VoxelData::const_iterator& b);

}


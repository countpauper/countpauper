#pragma once
#include <string>
#include "Voxel.h"
#include "Position.h"
#include "Direction.h"
#include "Box.h"
#include "Engine/Vector.h"
#include "Engine/Volume.h"

namespace Engine { struct AABB;  }
namespace Game
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
    Position Grid(const Engine::Coordinate& meters) const;
    Position ClippedGrid(const Engine::Coordinate& meters) const;
    Engine::Vector GridSize() const;
    Engine::Coordinate Center(const Position& p) const;
protected:
    template<class _D,typename _V>
    class base_iterator
    {
    public:
        // iterate over all data
        explicit base_iterator(_D& data) : base_iterator(data, data.Insides()) {}
        base_iterator(_D& data, const Box& box) : base_iterator(data, box, box.Start()) {}
        base_iterator(_D& data, const Box& box, const Position& position) : data(data), box(box), position(position)
        {
            // clip to end
            if (!box.Contains(position))
            {
                this->position = box.End();
            }
        }
        base_iterator<_D,_V>& operator++()
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
        //iterator operator++(int);
        bool operator==(const base_iterator<_D,_V>& other) const 
        {
            return &data == &other.data && position == other.position;
        }
        bool operator!=(const base_iterator<_D,_V>& other) const
        {
            return &data != &other.data || position != other.position;
        }

        using value_type = std::pair<Position, _V&>;
        value_type operator*() const
        {
            return std::pair<Position, _V&>(position, data[position]);
        }

        // iterator traits
        using difference_type = Position;
        using pointer = const value_type*;
        using reference = const value_type&;
        using iterator_category = std::forward_iterator_tag;
        _D& data;
        const Box box;
        Position position;
    };
public:
    using iterator = base_iterator<VoxelData, Voxel>;
    class const_iterator : public base_iterator<const VoxelData, const Voxel>
    {
    public:
        explicit const_iterator(const VoxelData& data) : base_iterator<const VoxelData, const Voxel>(data) {}
        explicit const_iterator(const VoxelData& data, const Box& box, const Position& position) : base_iterator<const VoxelData, const Voxel>(data, box, position) {}
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
    Position Clip(const Position& p) const;

    // Data getters and setters
    size_t Fill(const Engine::IVolume& v, const Material& m, double temperature);
    void SetPressure(const Position& location, const Material& material, double temperature, double pressure);
    void AdjustGrid(const Position& location, double temperature, double density);
    float Density(const Position& position) const;
    float Temperature(const Position& position) const;
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
    Engine::Vector grid;        // in meters, TODO: make it a transformation matrix grids to meters
    std::vector<Voxel> voxels;  // longitude+2, latitude+2, altitude+2
};


VoxelData::const_iterator::difference_type operator-(const VoxelData::const_iterator& a, const VoxelData::const_iterator& b);

}


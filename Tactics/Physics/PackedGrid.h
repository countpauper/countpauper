#pragma once
#include "VoxelGrid.h"
#include "Physics/PackedVoxel.h"

namespace Physics
{
    class PackedGrid : public VoxelGrid<PackedVoxel>
    {
    public:
        PackedGrid(const Engine::Vector& extent, const Grid& grid = Grid()) :
            VoxelGrid<PackedVoxel>(extent, grid)
        {
        }
        void Tick(double seconds);
    private:
        Box Flow(const Position& p, PackedVoxel& current);
        Box FlowDown(const Position& p, PackedVoxel& current);
        Box FlowSide(const Position& p, PackedVoxel& current);
        Box FlowUp(const Position& p, PackedVoxel& current);
        Box Evaporate(double seconds, const Position& position, PackedVoxel& current);
    };
}

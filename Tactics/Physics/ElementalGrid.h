#pragma once
#include "VoxelGrid.h"
#include "Physics/ElementalVoxel.h"
#include "Engine/UpdateFrequency.h"

namespace Physics
{
    class ElementalGrid : public VoxelGrid<ElementalVoxel>
    {
    public:
        ElementalGrid(const Engine::Vector& extent, const Grid& grid = Grid());
        void Tick(double seconds);
    private:
        void Update();
        void PropagateFire();
        void Sun(int heat);
        void Flow();
        void Evaporate();
        void Constrain();
        Engine::UpdateFrequency m_update;
    };
}

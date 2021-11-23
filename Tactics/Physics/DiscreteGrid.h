#pragma once
#include <string>
#include <array>
#include <map>
#include "IEnvironment.h"
#include "Engine/Vector.h"
#include "Physics/Grid.h"
#include "Physics/PackedVoxel.h"
#include "Physics/BoxIterator.h"

namespace Physics
{
class DiscreteGrid : public IEnvironment
{
public:
    // size in meter, grid in meter/voxel
    DiscreteGrid(const Engine::Vector& extent, const Grid& grid = Grid());

    size_t Fill(const Engine::IVolume& v, const Material& m, double temperature) override;
    void ApplyForce(const Engine::IVolume& c, const Engine::Vector& v) override;
    void ApplyForce(const Engine::Coordinate& c, double force) override;
    void Heat(const Engine::Coordinate& c, double energy) override;

    void ConnectChange(ChangeSignal::slot_type slot) override;
    double Density(const Engine::IVolume& c) const override;
    double Temperature(const Engine::IVolume& c) const override;
    Engine::Vector Force(const Engine::IVolume& c) const override;
    const Material* GetMaterial(const Engine::Coordinate& c) const override;
    Engine::RGBA Color(const Engine::Line& l) const override;
    void Tick(double seconds) override;
    double Measure(const Material* material) const override;
    std::wstring Statistics() const override;
    PackedVoxel& operator[](const Position& p);
    const PackedVoxel& operator[](const Position& p) const;

    using iterator = BoxDataIterator<DiscreteGrid, PackedVoxel>;
    using const_iterator = BoxDataIterator<const DiscreteGrid, const PackedVoxel>;
private:
    Box Bounds() const;
    unsigned Index(const Position& p) const;
    Size size;
    Grid grid;
    std::vector<PackedVoxel> m_data;
};
}
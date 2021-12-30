#pragma once
#include <string>
#include <array>
#include <map>

#include "IEnvironment.h"
#include "Engine/Vector.h"
#include "Physics/Grid.h"
#include "Physics/PackedVoxel.h"
#include "Physics/BoxIterator.h"
#include "Physics/Constraint.h"
#include "Physics/EnvironmentalEffects.h"

namespace Physics
{

class DiscreteGrid : public IEnvironment
{
public:
    // size in meter, grid in meter/voxel
    DiscreteGrid(const Engine::Vector& extent, const Grid& grid = Grid());

    size_t Fill(const Engine::IVolume& v, Filter filter, const Material& m, double temperature, std::optional<double> density=std::optional<double>()) override;
    void Constrain(const Engine::IVolume& v, const Material& m, double temperature, Function density) override;

    void ApplyForce(const Engine::IVolume& c, const Engine::Vector& v) override;
    void ApplyForce(const Engine::Coordinate& c, double force) override;
    void Heat(const Engine::Coordinate& c, double energy) override;

    is::signals::connection ConnectChange(ChangeSignal::slot_type slot) override;
    double Density(const Engine::IVolume& c) const override;
    double Temperature(const Engine::IVolume& c) const override;
    Engine::Vector Force(const Engine::IVolume& c) const override;
    const Material* GetMaterial(const Engine::Coordinate& c) const override;
    Engine::RGBA Color(const Engine::Line& l) const override;
    void Tick(double seconds) override;
    std::vector<const Engine::IRendition*> Render() const override;

    double Measure(const Material* material, const Engine::IVolume& in = Engine::AABB::infinity) const override;
    std::wstring Statistics() const override;
    PackedVoxel& operator[](const Position& p);
    const PackedVoxel& operator[](const Position& p) const;

    using iterator = BoxDataIterator<DiscreteGrid, PackedVoxel>;
    using const_iterator = BoxDataIterator<const DiscreteGrid, const PackedVoxel>;
private:
    Box Bounds() const;
    Box Neighbourhood(const Position& p) const;
    unsigned Index(const Position& p) const;
    Box Heat(double seconds, const Position& p, PackedVoxel& current);
    Box Flow(const Position& p, PackedVoxel& current);
    Box FlowDown(const Position& p, PackedVoxel& current);
    Box FlowSide(const Position& p, PackedVoxel& current);
    Box FlowUp(const Position& p, PackedVoxel& current);
    Box Evaporate(double seconds, const Position& position, PackedVoxel& current);
        
    double time = 0;    // TODO: could be in game and IEnvironment::Tick(...)
    Size size;
    Grid grid;
    std::vector<PackedVoxel> data;
    std::vector<Constraint> constraints;
    EnvironmentalEffects effects;
    ChangeSignal changed;
};
}

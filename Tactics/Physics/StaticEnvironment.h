#pragma once
#include <string>
#include "IEnvironment.h"
#include "VoxelData.h"
#include "Grid.h"

namespace Physics
{

class StaticEnvironment : public IEnvironment
{
public:
    // size in meter, grid in meter/voxel
    StaticEnvironment(const Engine::Vector& size, const Engine::Vector& grid=Engine::Vector(1,1,1));

    size_t Fill(const Engine::IVolume& v, const Material& m, double temperature) override;
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
    double Measure(const Material* material) const override;
    std::wstring Statistics() const override;
private:
    VoxelData data;
};
}

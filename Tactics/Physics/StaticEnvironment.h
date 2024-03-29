#pragma once
#include <string>
#include <optional>
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

    size_t Fill(const Engine::IVolume& v, Filter filter, const Material& m, double density, std::optional<double> temperature=std::optional<double>()) override;
    void Constrain(const Engine::IVolume& v, const Material& m, double temperature, Function density) override;
    void ApplyForce(const Engine::IVolume& c, const Engine::Vector& v) override;
    void Heat(const Engine::IVolume& c, double energy) override;

    is::signals::connection ConnectChange(ChangeSignal::slot_type slot) override;
    double Mass(const Engine::IVolume& c) const override;
    double Temperature(const Engine::IVolume& c) const override;
    Engine::Vector Force(const Engine::Coordinate& c) const override;
    const Material* GetMaterial(const Engine::Coordinate& c) const override;
    Engine::RGBA Color(const Engine::Line& l) const override;
    void Tick(double seconds) override;
    std::vector<const Engine::IRendition*> Render() const override;
    double Measure(const Material* material, const Engine::IVolume& in = Engine::AABB::infinity) const override;
    std::string Statistics() const override;
private:
    VoxelData data;
};
}

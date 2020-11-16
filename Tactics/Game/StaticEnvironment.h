#pragma once
#include <string>
#include "IEnvironment.h"
#include "VoxelData.h"
// impl
#include "Box.h"

namespace Game
{

class StaticEnvironment : public IEnvironment
{
public:
    StaticEnvironment(const Size& size);

    void Fill(const Engine::IVolume& v, const Material& m) override;
    void ApplyForce(const Engine::IVolume& c, const Engine::Vector& v) override;
    void ApplyForce(const Engine::Coordinate& c, double force) override;
    void Heat(const Engine::Coordinate& c, double energy) override;

    void ConnectChange(ChangeSignal::slot_type slot) override;
    double Density(const Engine::IVolume& c) const override;
    double Temperature(const Engine::IVolume& c) const override;
    Engine::Vector Force(const Engine::IVolume& c) const override;
    const Material* GetMaterial(const Engine::Coordinate& c) const override;
private:
    VoxelData data;
};
}
#pragma once

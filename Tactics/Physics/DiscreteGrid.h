#pragma once
#include "IEnvironment.h"
#include "Engine/Vector.h"
#include "Physics/Grid.h"
#include "Physics/Constraint.h"
#include "Physics/EnvironmentalEffects.h"

namespace Physics
{

class DiscreteGrid : public IEnvironment
{
public:
    // size in meter, grid in meter/voxel
    DiscreteGrid(const Engine::Vector& extent, const Grid& grid = Grid());
    is::signals::connection ConnectChange(ChangeSignal::slot_type slot) override;
    std::vector<const Engine::IRendition*> Render() const override;
    void Constrain(const Engine::IVolume& v, const Material& m, double temperature, Function density) override;
protected:
    Box Bounds() const;
    Box Neighbourhood(const Position& p) const;
    unsigned Index(const Position& p) const;

    Size size;
    Grid grid;
    double time = 0;    // TODO: could be in game and IEnvironment::Tick(...)
    std::vector<Constraint> constraints;
    EnvironmentalEffects effects;
    ChangeSignal changed;
};

}

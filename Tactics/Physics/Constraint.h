#pragma once
#include "Engine/Volume.h"
#include "IEnvironment.h"
#include "Engine/AxisAlignedBoundingBox.h"
#include <functional>
#include "Engine/Clone.h"

namespace Physics
{
struct Material;

class Constraint
{
public:
    Constraint() = default;
    Constraint(const Engine::IVolume& v, const Material& material, double temperature, Function f);
    void Tick(double t, IEnvironment& environment) const;
private:
    const Material * material;
    Engine::clone_ptr<Engine::IVolume> volume;
    double temperature;
    Function fn;
};

}
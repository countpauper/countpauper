#pragma once
#include "Engine/Volume.h"
#include "IEnvironment.h"

#include <functional>

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
    Engine::AABox area;
    double temperature;
    Function fn;
};

}
#pragma once
#include "Engine/Volume.h"

#include <functional>

namespace Physics
{
struct Material;
class IEnvironment;

class Condition
{
public:
    Condition() = default;
    using Function = std::function<double(double)>;
    Condition(const Material& material, const Engine::AABox& area, double temperature, Function f);
    void Tick(double t, IEnvironment& environment);
private:
    const Material * material;
    Engine::AABox area;
    double temperature;
    Function fn;
};

}
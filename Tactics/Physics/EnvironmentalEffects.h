#pragma once

#include "Engine/Coordinate.h"
#include "Engine/Effect.h"
#include <memory>

namespace Physics
{
struct Material;

class EnvironmentalEffect
{
public:
    EnvironmentalEffect() = default;
    EnvironmentalEffect(const Engine::Coordinate& origin, const Material& material, double mass, double temperature);
   
    void Tick(double seconds);
    const Engine::Particles& GetParticles() const;
private:
    const Material* material = nullptr;
    double mass = 0;
    double temperature = 0;
protected:
    std::unique_ptr<Engine::Particles> particles;
};

class Steam : public EnvironmentalEffect
{
public:
    Steam(const Engine::Coordinate& origin, double mass, double temperature);
};

class EnvironmentalEffects : public std::vector<EnvironmentalEffect>
{
public:
    void Tick(double seconds);
};
}
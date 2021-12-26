#include "stdafx.h"
#include "DiscreteGrid.h"
#include "Engine/Debug.h"
#include "Engine/Volume.h"
#include "Direction.h"
#include "EnvironmentalEffects.h"

#include <sstream>

namespace Physics
{

    EnvironmentalEffect::EnvironmentalEffect(const Engine::Coordinate& origin, const Material& material, double mass, double temperature) :
        material(&material),
        mass(mass),
        temperature(temperature)
    {
    }

    void EnvironmentalEffect::Tick(double seconds)
    {
        particles->Tick(seconds);
    }

    const Engine::Particles& EnvironmentalEffect::GetParticles() const
    {
        return *particles;
    }


    void EnvironmentalEffects::Tick(double seconds)
    {
        for (auto& effect : *this)
            effect.Tick(seconds);
    }


    Steam::Steam(const Engine::Coordinate& origin, double mass, double temperature) :
        EnvironmentalEffect(origin, Material::water, mass, temperature)
    {
        const double massPerParticle = 1;
        particles = std::make_unique<Engine::ParticleEffect<Engine::Cloud>>(static_cast<size_t>(mass/massPerParticle)); // mass / massPerParticle);
        particles->m.SetTranslation(Engine::Vector(origin.x, origin.z, origin.y));  // TODO y/z swap find the right place
        // TODO: mass and temperature influence particle amount/movement
    }


}

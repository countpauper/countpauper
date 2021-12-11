#pragma once

#include "Coordinate.h"
#include "Color.h"
#include "Vector.h"
#include "Matrix.h"
#include "Random.h"
#include "AxisAlignedBoundingBox.h"
#include <vector>
#include <memory>

namespace Engine
{
    
    struct Particle
    {
        Coordinate c;
        Vector movement;
        double size = 1;
        double age = 0;
        RGBA color;

        void Despawn();
        bool IsSpawned() const;
    };



    class Particles
    {
    public:
        Particles(std::unique_ptr<class Effect> effect);
        Particle& Spawn();
        void Render() const;
        void Tick(double seconds);
        size_t Count() const;
        AABB Bounds() const;

        double age = 0;
        Matrix m;
        std::unique_ptr<class Effect> effect;
        std::vector<Particle> particles;    
    };

    template<class E, typename... T> 
    class ParticleEffect : public Particles
    {
    public:
        ParticleEffect(T... parameters) :
            Particles(std::make_unique<E>(std::forward(parameters)...))
        {
        }
    };

    class Effect
    {
    public:
        virtual void Tick(double seconds, Particles& particles) const = 0;
        virtual void Tick(double seconds, Particle& particle) const = 0;
    protected:
        static Generator random;
    };

    class Steam: public Effect
    {
    public:
        void Tick(double seconds, Particles& particles) const override;
        void Tick(double seconds, Particle& particle) const override;
    private:
        const double rate = 1;  // Hz, particles spawned per second
        const double lifetime = 10; // Maximimum particle age in seconds
        const double speed = 0.5; // vertical speed
        const double zag = 0.2; // horizontal zig zag
    };

}   // ::Engine
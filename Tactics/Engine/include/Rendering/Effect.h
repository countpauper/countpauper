#pragma once

#include "Geometry/Coordinate.h"
#include "Rendering/Color.h"
#include "Geometry/Vector.h"
#include "Geometry/Matrix.h"
#include "Utility/Random.h"
#include "Geometry/AxisAlignedBoundingBox.h"
#include "Rendering/IRendition.h"
#include <vector>
#include <map>
#include <memory>

namespace Engine
{

    struct Particle
    {
        Coordinate c;
        Vector movement;
        double size = 1;
        double age = 0;
        RGBA color{0};

        void Despawn();
        bool IsSpawned() const;
    };



    class Particles : public IRendition
    {
    public:
        Particles(std::unique_ptr<class Effect> effect);
        Particle& Spawn();
        void Render() const override;
        void Tick(double seconds);
        size_t Count() const;
        AABB Bounds() const;

        double age = 0;
        Matrix m;
        std::unique_ptr<class Effect> effect;
        std::vector<Particle> particles;
        static void LoadTextures(const std::string_view location);
    private:
        static std::map<std::string, class Texture> textures;
    };

    class Effect
    {
    public:
        virtual ~Effect() = default;
        virtual void Tick(double seconds, Particles& particles) const = 0;
        virtual void Tick(double seconds, Particle& particle) const = 0;
    protected:
        static Generator random;
    };

    class Cloud : public Effect
    {
    // https://www.weather.gov/source/zhu/ZHU_Training_Page/clouds/cloud_development/clouds.htm
    public:
        Cloud(size_t count);
        void Tick(double seconds, Particles& particles) const override;
        void Tick(double seconds, Particle& particle) const override;
    private:
        const size_t count;
        const double rate = 1;  // Hz, particles spawned per second
        const double lifetime = 10; // Maximimum particle age in seconds
        const double speed = 0.5; // vertical speed
        const double zag = 2; // horizontal zig zag
    };

    template<class E>
    class ParticleEffect : public Particles
    {
    public:
        template<typename... T>
        explicit ParticleEffect(T... parameters) :
            Particles(std::make_unique<E>(std::forward<T>(parameters)...))
        {
        }
        ParticleEffect(const ParticleEffect<E>&) = delete;
        ParticleEffect(ParticleEffect<E>&&) = delete;
    };



}   // ::Engine

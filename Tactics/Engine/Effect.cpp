#include "stdafx.h"
#include "Effect.h"
#include <GL/glew.h>
#include "Drawing.h"
#include "Random.h"

namespace Engine
{
    void Particle::Despawn()
    {
        age = std::numeric_limits<decltype(age)>::quiet_NaN();
    }

    bool Particle::IsSpawned() const
    {
        return age >= 0;
    }

    Particles::Particles(std::unique_ptr<Effect> effect) :
        effect(std::move(effect))
    {
        m = Matrix::Identity();

    }

    Particle& Particles::Spawn()
    {
        particles.emplace_back(Particle());
        return particles.back();
    }

    void Particles::Render() const
    {
        auto pm = Matrix::Projection();
        glPushMatrix();
        glMultMatrixd(&m.data());

        glBegin(GL_QUADS);
        for (const auto& p : particles)
        {
            p.color.Render();
            glVertex(Vector(p.c) + TransformBillboard(Vector(0,           0, 0), pm));
            glVertex(Vector(p.c) + TransformBillboard(Vector(p.size,      0, 0), pm));
            glVertex(Vector(p.c) + TransformBillboard(Vector(p.size, p.size, 0), pm));
            glVertex(Vector(p.c) + TransformBillboard(Vector(     0, p.size, 0), pm));
        }
        glEnd();
        glPopMatrix();
    }

    void Particles::Tick(double seconds)
    {
        effect->Tick(seconds, *this);
        for (auto& p : particles)
        {
            effect->Tick(seconds, p);
            p.age += seconds;
            p.c += p.movement * seconds;
        }
        age += seconds;
        particles.erase(std::remove_if(particles.begin(), particles.end(), [](const Particle& p)
        {
            return !p.IsSpawned();
        }), particles.end());
    }

    size_t Particles::Count() const
    {
        return particles.size();
    }

    AABB Particles::Bounds() const
    {
        AABB bounds = AABB::empty;
        for (const auto& p : particles)
        {
            bounds |= p.c;
        }
        return m * bounds;
    }

    void Steam::Tick(double seconds, Particles& particles) const
    {
        if (particles.Count() <= int(particles.age * rate))
        {
            auto& p = particles.Spawn();
            p.color = RGBA::white;
            p.movement.y = speed;
            p.size = 0.1;
        }

    }

    void Steam::Tick(double seconds, Particle& particle) const
    {
        if (particle.age > lifetime)
            particle.Despawn();
        double zig = seconds * zag;   // take of every zig
        particle.movement.x += random.Uniform(Range<double>(-zig, zig));
        particle.movement.z += random.Uniform(Range<double>(-zig, zig));
        particle.movement.y -= zig*(0.5/lifetime);

        particle.color = RGBA::white.Translucent(1.0-(particle.age / lifetime));
    }

Generator Effect::random;

}
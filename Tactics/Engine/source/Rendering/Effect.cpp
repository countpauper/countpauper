#include "Rendering/Effect.h"
#include "Rendering/Drawing.h"
#include "Utility/Random.h"
#include "Rendering/Texture.h"
#include "Utility/Utils.h"
#include <filesystem>
#include <GL/gl.h>

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
        auto bind = textures["flame_03"].Bind();

        auto pm = Matrix::Projection();
        glPushMatrix();
        glMultMatrixd(&m.data());

        //glColor3d(1, 1, 1);
        //Bounds().Render();

        glBegin(GL_QUADS);
        for (const auto& p : particles)
        {
            p.color.Render();
            glTexCoord2f(0, 0);
            glVertex(Vector(p.c) + TransformBillboard(Vector(0, 0, 0), pm));
            glTexCoord2f(0, 1);
            glVertex(Vector(p.c) + TransformBillboard(Vector(p.size, 0, 0), pm));
            glTexCoord2f(1, 1);
            glVertex(Vector(p.c) + TransformBillboard(Vector(p.size, p.size, 0), pm));
            glTexCoord2f(1, 0);
            glVertex(Vector(p.c) + TransformBillboard(Vector(0, p.size, 0), pm));
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

    Cloud::Cloud(size_t count):
        count(count)
    {
    }
    void Cloud::Tick(double seconds, Particles& particles) const
    {
        for (size_t i = particles.Count(); i < count; ++i)
        {
            auto& p = particles.Spawn();
            p.c.x = Effect::random.Chance();
            p.c.z = Effect::random.Chance();
            p.color = RGBA::white;
            p.size = 0.25;
        }
        particles.m *= Matrix::Translation(Vector(0, speed * seconds, 0));
    }

    void Cloud::Tick(double seconds, Particle& particle) const
    {
        if (particle.age > lifetime)
            particle.Despawn();
        double zig = seconds * zag;   // Move 'ZIG'
        particle.movement.x += random.Uniform(Range(-zig, zig));
        particle.movement.y += random.Uniform(Range(-zig, zig));
        particle.movement.z += random.Uniform(Range(-zig, zig));

        particle.color = RGBA::white.Translucent(1.0-(particle.age / lifetime));
    }


    void Particles::LoadTextures(std::string_view location)
    {
        for (const auto & entry : std::filesystem::directory_iterator(location))
        {
            if (!entry.is_regular_file())
                continue;
            if (UpperCase(entry.path().extension().string()) != ".PNG")
                continue;

            textures.emplace(std::make_pair(entry.path().stem().string(), Image(entry.path().string())));
        }
    }


Generator Effect::random;
std::map<std::string, Texture> Particles::textures;

}

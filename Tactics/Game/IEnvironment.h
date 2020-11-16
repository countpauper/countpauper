#pragma once
#include <string>
#include "Direction.h"
#include "Engine/Volume.h"
#include "Material.h"
#include <signal.h>

namespace Game
{

class IEnvironment
{
public:
    virtual void Fill(const Engine::IVolume& v, const Material& m)=0;
    virtual void ApplyForce(const Engine::IVolume& c, const Engine::Vector& v)=0;
    virtual void ApplyForce(const Engine::Coordinate& c, double force) = 0;
    virtual void Heat(const Engine::Coordinate& c, double energy) = 0;
    using ChangeSignal = is::signals::signal<void(const Engine::AABB&)>;

    virtual void ConnectChange(ChangeSignal::slot_type slot) = 0;
    virtual double Density(const Engine::IVolume& c) const =0;
    virtual double Temperature(const Engine::IVolume& c) const = 0;
    virtual Engine::Vector Force(const Engine::IVolume& c) const = 0;
    virtual const Material* GetMaterial(const Engine::Coordinate& c) const = 0;

};
}

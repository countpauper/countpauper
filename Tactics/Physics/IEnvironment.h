#pragma once
#include "Material.h"
#include "Filter.h"
#include <signal.h>
#include <vector>

namespace Engine { 
    struct Line; 
    struct Coordinate;
    struct Vector;  
    class IVolume; 
    struct AABB;
    class IRendition;
}

namespace Physics
{
using Function = std::function<double(double)>;


class IEnvironment
{
public:
    // Creation
    virtual size_t Fill(const Engine::IVolume& v, Filter filter, const Material& m, double temperature, std::optional<double> density=std::optional<double>()) = 0;
    virtual void Constrain(const Engine::IVolume& v, const Material& m, double temperature, Function density) = 0;
    virtual void ApplyForce(const Engine::IVolume& c, const Engine::Vector& v)=0;
    virtual void Heat(const Engine::Coordinate& c, double energy) = 0;
    using ChangeSignal = is::signals::signal<void(const Engine::AABB&)>;

    virtual is::signals::connection ConnectChange(ChangeSignal::slot_type slot) = 0;
    virtual double Density(const Engine::IVolume& c) const =0;
    virtual double Temperature(const Engine::IVolume& c) const = 0; // kelvin
    virtual Engine::Vector Force(const Engine::Coordinate& c) const = 0;
    virtual const Material* GetMaterial(const Engine::Coordinate& c) const = 0;
    //    virtual std::pair<double, const Material*> Collision(const Engine::Line& line) const = 0;
    virtual Engine::RGBA Color(const Engine::Line& l) const = 0;
    virtual double Measure(const Material* material, const Engine::IVolume& v) const = 0; // const Engine::IVolume* v = nullptr) const = 0;
    virtual std::wstring Statistics() const = 0;
    virtual void Tick(double seconds) = 0;
    virtual std::vector<const Engine::IRendition*> Render() const = 0;
};
}

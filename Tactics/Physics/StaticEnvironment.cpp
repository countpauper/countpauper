#include "stdafx.h"
#include "StaticEnvironment.h"
#include "Engine/Line.h"

namespace Physics
{
    
StaticEnvironment::StaticEnvironment(const Engine::Vector& size, const Engine::Vector& grid) :
    data(Size(int(std::round(size.x / grid.x)), int(std::round(size.y/ grid.y)), int(std::round(size.z / grid.z))), 1, grid)
{
    for (auto& v : data)
    {
        float temperature = 290.0;
        v.second = { &Material::vacuum, temperature, 0 };
    }

}

size_t StaticEnvironment::Fill(const Engine::IVolume& v, Filter filter, const Material& m, double temperature, std::optional<double> density)
{
    return data.Fill(v, filter, m, temperature, density.has_value()?*density : m.Density(PascalPerAtmosphere, temperature));
}


void StaticEnvironment::Constrain(const Engine::IVolume& v, const Material& m, double temperature, Function density)
{
}

void StaticEnvironment::ApplyForce(const Engine::IVolume& v, const Engine::Vector& force) {}
void StaticEnvironment::Heat(const Engine::IVolume& v, double energy) {}

is::signals::connection StaticEnvironment::ConnectChange(ChangeSignal::slot_type slot) { return is::signals::connection(); }

double StaticEnvironment::Mass(const Engine::IVolume& c) const
{
    return data.Mass(c);
}

double StaticEnvironment::Temperature(const Engine::IVolume& c) const
{
    return data.Temperature(c);
}

Engine::Vector StaticEnvironment::Force(const Engine::Coordinate& c) const
{
    return data.Force(c);
}

const Material* StaticEnvironment::GetMaterial(const Engine::Coordinate& c) const
{
    // TODO: grid size in the environment or the data
    Position p(data.grid(c));
    if (!data.IsInside(p))
        return nullptr;
    return &data.MaterialAt(p);
}

Engine::RGBA StaticEnvironment::Color(const Engine::Line& l) const
{
    Position p(data.grid(l.b));
    if (!data.IsInside(p))
        return Engine::RGBA::transparent;
    return data[p].Color(l.Length());
}

void StaticEnvironment::Tick(double seconds)
{
}

std::vector<const Engine::IRendition*> StaticEnvironment::Render() const
{
    return std::vector<const Engine::IRendition*>();
}


double StaticEnvironment::Measure(const Material* material, const Engine::IVolume& in) const
{
    return data.Measure(material, in);
}


std::string StaticEnvironment::Statistics() const
{
    return "TODO: Voxeldata statistics";
}


}
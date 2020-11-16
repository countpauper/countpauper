#include "stdafx.h"
#include "StaticEnvironment.h"

namespace Game
{
    
StaticEnvironment::StaticEnvironment(const Size& size) :
    data(size){
    for (auto& v : data)
    {
        float temperature = 290.0;
        v.second = { &Material::air, temperature, float(Material::air.Density(PascalPerAtmosphere, temperature)) };
    }
}

void StaticEnvironment::Fill(const Engine::IVolume& v, const Material& m) 
{

    auto bb = v.GetBoundingBox();
    Box volBB(Position(int(bb.Begin().x), int(bb.Begin().y), int(bb.Begin().z)),
        Position(int(ceil(bb.End().x)), int(ceil(bb.End().y)), int(ceil(bb.End().z))));
    for (auto& voxel: data.In(volBB))
    {
        Engine::Coordinate center(voxel.first.x, voxel.first.y, voxel.first.z); // TODO: grid
        center += Engine::Vector(0.5, 0.5, 0.5);
        if (v.Distance(center) <= 0)
        {
            data[voxel.first] = { &m, voxel.second.temperature, float(m.Density(PascalPerAtmosphere, voxel.second.temperature)) };
        }
    }
}

void StaticEnvironment::ApplyForce(const Engine::IVolume& c, const Engine::Vector& v) {}
void StaticEnvironment::ApplyForce(const Engine::Coordinate& c, double force) {}
void StaticEnvironment::Heat(const Engine::Coordinate& c, double energy) {}

void StaticEnvironment::ConnectChange(ChangeSignal::slot_type slot) {}

double StaticEnvironment::Density(const Engine::IVolume& c) const
{
    return 0;
}

double StaticEnvironment::Temperature(const Engine::IVolume& c) const
{
    return 0;
}

Engine::Vector StaticEnvironment::Force(const Engine::IVolume& c) const
{
    return Engine::Vector();
}

const Material* StaticEnvironment::GetMaterial(const Engine::Coordinate& c) const
{
    // TODO: grid size in the environment or the data
    Position p(int(c.x), int(c.y), int(c.z));
    if (!data.IsInside(p))
        return nullptr;
    return &data.MaterialAt(p);
}

}
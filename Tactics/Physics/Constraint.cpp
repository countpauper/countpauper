#include "stdafx.h"
#include "Physics/Constraint.h"
#include "Engine/Debug.h"

namespace Physics
{

Constraint::Constraint(const Engine::IVolume& v, const Material& mat,double temp, Function f) :
    material(&mat),
    volume(v),   
    temperature(temp),
    fn(f)
{

}

void Constraint::Tick(double t, IEnvironment& environment) const
{
    Filter materialFilter = [this](const Engine::Coordinate&, const Material* m, double, double)
    {
        return m == material;
    };
    if (fn)
    {
        auto count = environment.Fill(*volume, materialFilter, *material, temperature, fn(t));
        Engine::Debug::Log(std::string("Constrained ") + std::to_string(count) + " blocks of " + std::string(material->name)+ " in a " + std::to_string(volume->Volume()) + " volume.");
    }
}

}

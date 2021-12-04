#include "stdafx.h"
#include "Physics/Constraint.h"

namespace Physics
{

Constraint::Constraint(const Engine::IVolume& v, const Material& mat,double temp, Function f) :
    material(&mat),
    area(v.GetBoundingBox()),   // TODO: not exactly right, need a way to clone volumes
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
        environment.Fill(area, materialFilter, *material, temperature, fn(t));
}


}

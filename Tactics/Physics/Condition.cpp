#include "stdafx.h"
#include "Physics/Condition.h"
#include "Physics/IEnvironment.h"

namespace Physics
{

Condition::Condition(const Material& mat, const Engine::AABox& area, double temp, Function f) :
    material(&mat),
    area(area),
    temperature(temp),
    fn(f)
{

}

void Condition::Tick(double t, IEnvironment& environment)
{
    if (fn)
        environment.Fill(area, *material, fn(t), 300);
}


}

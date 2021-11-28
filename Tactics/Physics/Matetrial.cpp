#include "stdafx.h"
#include "Material.h"
#include "Engine/from_string.h"

namespace Physics
{


bool Material::Solid(double temperature) const
{
    return temperature < melt;
}

bool Material::Fluid(double temperature) const
{
    return temperature >= melt && temperature < boil;
}

bool Material::Gas(double temperature) const
{
    return temperature >= boil;
}


double Material::Density(double pressure, double temperature) const
{
    if (temperature > boil)
    {
        return (molarMass * pressure) / (temperature * IdealGasConstant);
    }
    else
    {
        return normalDensity;  // incompressible, also as liquid
    }
}

std::ostream& operator<<(std::ostream& stream, const Physics::Material& material)
{
    stream << Engine::from_string<std::string>(material.name);
    return stream;
}

std::ostream& operator<<(std::ostream& stream, const Physics::Material* material)
{
    if (!material)
        stream << "None";
    else
        stream << *material;
    return stream;
}

}

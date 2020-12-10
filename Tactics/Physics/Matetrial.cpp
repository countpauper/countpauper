#include "stdafx.h"
#include "Material.h"

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

}

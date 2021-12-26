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

double Material::Evaporation(double temperature, double pressure, double humidity, double windSpeed) const
{
    // https://en.wikipedia.org/wiki/Clausius%E2%80%93Clapeyron_relation
    // that's too hard so approximation, which currently uses constants that are water specific
    assert(this == &Material::water);
    // https://www.engineeringtoolbox.com/evaporation-water-surface-d_690.html
    // Θ A (xs - x) / 3600  
    // Θ = 44 (35+1m/s * 19) 
    // A = 1 m2 (because that's the unit 

    // assuming v= 1m/s wind to carry vapor away, could take velocity from air block when it's tracked
    const double theta = 25 + windSpeed * 19.0;
    const double A = 1;
    const double secondsPerHour = 3600.0;
    // Humidity ratio(xs) at this temperature: https://www.engineeringtoolbox.com/humidity-ratio-air-d_686.html
    double pws = exp(77.3450 + 0.0057 * temperature - 7235 / temperature) / pow(temperature, 8.2);
    // Using 1 bar atmoshpheric pressure here. It should be the pressure of the air evaporating t;
    double xs = 0.62198 * pws / (pressure - pws);
    // current humidity assumed to be constant 1g/kg, since it's not tracked in air currently. 
    // perhaps it could be based on overlapping vapor particles or 
    // even packed voxels should track humidity in air (and mineral) and dirt is just wet + mineral (assume bio mass grows instantly) 
    const double x = humidity;
    double evaporation = theta * A * (xs - x) / 3600;
    return 1000.0 * evaporation;
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

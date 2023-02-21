#include "stdafx.h"
#include "Material.h"
#include "Engine/from_string.h"
#include "Engine/Maths.h"

namespace Physics
{

//                               Name,         Color,                      Melt,   Boil, Density, molar mass,  Viscosity,  Conduct,    thermalCond, Capacity    surfaceTensiom, youngModulus, granularit,  Opacity,         
const Material Material::vacuum{ "Vacuum",    Engine::RGBA(0x00000000),   0,      0,      0,      0,           0,          0,          0,          0,          0,              0,             0.0,          0.0 };
const Material Material::air   { "Air",       Engine::RGBA(0xFFA08040),   60,     80,     1.225,  29,          18e-6,      29.2,       0.024,      1.012,      10e-3,          1e9,           0.0,          0.01 };     // 28.964g/mol because n2&o2 diatomic
const Material Material::soil  { "Soil",      Engine::RGBA(0xFF20FF20),   1000,   0,      1600,   65,          10e6,       0.4,        1,          2.0,        0.7,            1e7,           1000.0,       10 };     // 65g/mol, based on 0% humidity. Part SiO2, N2 and proteins
const Material Material::stone { "Stone",     Engine::RGBA(0xFFA0A0A0),   1986,   3220,   2648,   60,          10e21,      10,         1.8,        0.790,      0.8,            1e10,          0.0,          10 };     // for now 100% silicon dioxide, 60 g/mol
const Material Material::water { "Water",     Engine::RGBA(0xFFFF6010),   273,    373,    1000,   18,          8.9e-4,     0.6065,     0.6,        4.1813,     0.07,           9e9,           0.0,          6.9 / 1000 };     // H2O 18 g/mol
                                                                                                                                            
// redundant for now VoxelMap::Material VoxelMap::Material::sand  { L"Sand",      1986,   3220,   2648,        10,            0.0           };     // silicon dioxide, 60 g/mol

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

bool Material::Granular() const
{   // assuming not gas or liquid 
    return granularity > 0;
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

double Material::Viscosity(double density, double temperature) const
{
    assert(density > 0.1*normalDensity && density < 10 * normalDensity);
    assert(temperature > 270 && temperature < 300);
    // TODO: it's very temperature temperature dependendent, there are special formulas for 
    // water : https://en.wikipedia.org/wiki/Viscosity#Water
    // air   : https://en.wikipedia.org/wiki/Viscosity#Air
    //  assuming they are both still liquid and gasseous respectively
    // stone and soil would also have different numbers in liquid phase. eg lava is more like 10e4 - 10e8 depending also on the silicate content
    return this->viscosity;
}

double Material::Reynolds(double density, double temperature, double velocity) const
{
   // Has to do with viscosity VS friction https://en.wikipedia.org/wiki/Reynolds_number
   // ELI5: https://www.youtube.com/watch?v=wtIhVwPruwY&list=PLt5AfwLFPxWK_BKKR2xTZoh2MddOtGbAM&index=2&t=2s
   // TLDR: High numbers (>1000) pressure is dominant, low numbers (<0.01) internal friction is dominant
   //       It's made up of density(kgm-3) * length(m) * velocity(ms-1) / viscosity(kgm-1s-1)
   //       If any of the first terms is very high or the viscosity is low (the fluid is dense, the scale is big of the speed is high and or the fluid is not sticky) 
   //         The reynolds number will be high (>1000) and pressure is dominant, friction negligable
   //        Conversely at small scale, high density or low velocity or very sticky, the number may be low (<0.01) and pressure is negligable, friction is dominant 
   //  The scale here is fixed (~one meter per grid), speeds are up to 0-10m/s so the material (density and viscosity) is dominant
   // TODO: per grid reynold's number based on at least material. perhaps scale with current velocity and pressure 

    const double scale = 0.5;   // ~average between vertical and horizontal meter per grid 
    return density * scale * velocity / Viscosity(density, temperature);

}

Engine::RGBA Material::Color(double temperature, double density) const
{
    double translucency = 1.0;
    if (Fluid(temperature))
        translucency = 0.2 + std::min(0.8, density / 2500.0);
    else if (Gas(temperature))
    {
        static const Engine::RGBA redFire   { 255, 32, 16, 32 };
        static const Engine::RGBA orangeFire{ 255, 128, 32, 64 };
        static const Engine::RGBA whiteFire { 255, 255, 255, 128 };
        static const Engine::RGBA blueFire  { 32, 32, 255, 64 };

        if (temperature <= 900)
            return Engine::RGBA::transparent;
        else if (temperature <= 1250)
            return Engine::Lerp(900, redFire, 1250, orangeFire, temperature);
        else if (temperature <= 1750)
            return Engine::Lerp(1250, orangeFire, 2000, whiteFire, temperature);
        else
            return Engine::Lerp(2000, whiteFire, 3500, blueFire, temperature);
    }
    auto baseColor = color.Translucent(translucency);
    if (temperature < 270)
        return Engine::Lerp(baseColor, Engine::RGBA(0, 0, 255), (temperature - 270) / 270.0);
    else if (temperature > 750)
        return Engine::Lerp(baseColor, Engine::RGBA(255, 64, 0), (temperature - 750) / (boil - 700));
    else
        return baseColor;

}

std::ostream& operator<<(std::ostream& stream, const Physics::Material& material)
{
    stream << material.name;
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

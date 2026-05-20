#include "Game/Material.h"
#include "Utility/Maths.h"
#include <limits>

namespace Game
{


bool Material::operator==(const Material& other) const
{
    return this == &other;
}
static constexpr float NaN = std::numeric_limits<float>::quiet_NaN();

/// @brief                          Name            Color, alpha = opacity(/255) per meter      freeze     boil(K)   Density  Density(@boil)   molemass    Enthalpy  liq visc,  gas visc
//                                                                                                 (K)       (K)     (kg/m3)            (kg/mol)      (J/mol)    (Pa*s)   (Pa*s)
const Material Material::air    {   "air",          Engine::RGBA(0x40, 0x40, 0xF0, 0x0  ),        63.0f,    83.0f,   1026.0f,  870.0f,  28.695e-3f,   11570,    1e-4,    18e-6 };
const Material Material::water  {   "water",        Engine::RGBA(0x70, 0x90, 0xc0, 0xC0 ),       273.15f,  373.15f,  1000.0f,  990.0f,   18.020e-3f,   40660,    1e-3,    12e-6 };
const Material Material::earth  {   "dirt",         Engine::RGBA(0x80, 0x40, 0x20       ),       800.0f,  1100.0f,   1600.0f,  2200.0f, 112.0e-3f,    168000,    1e+5,    20e-6 };
const Material Material::stone  {   "stone",        Engine::RGBA(0x60, 0x60, 0x60       ),      1250.0f,  3000.0f,   2700.0f,  2500.0f,  81.585e-3f,   49000,    1e+3,    20e-6 };
const Material Material::vegetation {"vegetation",  Engine::RGBA( 0x0, 0xA0,  0x0       ),       250.0f,   400.0f,   1100.0f,  1000.0f, 300.0e-3f,     72000,    1.0,     10e6 };
const Material Material::vacuum {   "vacuum",       Engine::RGBA( 0x0,  0x0,  0x0, 0x0  ),          NaN,      NaN,   0.0f,     0.0f,      0.0e-3f,       NaN,    0.0,     0.0 };
const Material* Material::all[] = { &air, &water, &earth, &stone, &vegetation };

// NB dirt behaves like rhyoitic lava when melted in visosity and enthalpy
// TODO: See tables file for blood, oil, metal, wood etc

float Material::PhaseFactor(float temperature, float pressure) const
{   
    auto freeze = FreezingPoint(pressure);
    auto boil = BoilingPoint(pressure);
    return (temperature-freeze)/(boil-freeze);
}

float Material::PhaseFactorSimple(float temperature) const
{   // simpler and faster version that doesn't take pressure into account
    auto freeze = freezingPoint;
    auto boil = boilingPoint;
    return (temperature-freeze)/(boil-freeze);
}

float Material::Density(float temperature, float pressure) const
{
    if (molarmass==0.0)
        return 0.0;
    // TODO very rough estimates, use ideal gas law and adjust liquids/solids as well with K value 
    float phase = PhaseFactor(temperature, pressure);
    if (phase<=0)   // solid
        return solidDensity; 
    else if (phase>1)   // gas
        return (pressure * molarmass / (gasConstant * temperature));
    else // liquid. density increase due to pressure is insignificant (0.5% per atm for oil, much less for water)
        return Engine::Lerp(solidDensity, liquidDensity, phase);    
}

float Material::Pressure(float temperature, float density) const
{
    if (molarmass==0.0)
        return 0.0;
    float phase = PhaseFactorSimple(temperature);
    if (phase<=0)   // solid
        return std::numeric_limits<float>::quiet_NaN(); 
    else if (phase>1)   // gas
        return (density / molarmass) * gasConstant * temperature;
    else 
        return atmosphericPressure * density / Density(temperature, atmosphericPressure);   
}

float Material::FreezingPoint(float pressure) const
{
    // The effect of pressure is negligeble for the freezing point 
    return freezingPoint;
}

float Material::BoilingPoint(float pressure) const
{
    // https://en.wikipedia.org/wiki/Clausius%E2%80%93Clapeyron_relation#Chemistry_and_chemical_engineering
    // TODO: Since this is called a lot and heavy, consider a lookup table and/or
    // split materials in phases with their own properties and make it a state change for layers. 
    float denom = (1.0f / boilingPoint) - (gasConstant/enthalpy) * std::log(pressure / atmosphericPressure);
    denom = std::max(denom, 2e-4f); // clip boiling point to 5000K (since Cell::Temperature goes to 8k and this approximation can go to infinity)
    return 1.0f / denom;
}

bool Material::IsSolid(float temperature, float pressure) const
{
    return temperature <= FreezingPoint(pressure);
}

bool Material::IsLiquid(float temperature, float pressure) const
{
    return temperature >  FreezingPoint(pressure) && temperature <= BoilingPoint(pressure);
}

bool Material::IsGas(float temperature, float pressure) const
{
    return temperature > BoilingPoint(pressure);
}

float Material::Viscosity(float temperature, [[maybe_unused]] float pressure) const
{
    float phase = PhaseFactorSimple(temperature);   // Simple phase is used, no pressure, for speed and because it doesn't have a noticable effect on viscosity. 
    if (phase<=0)   // solid
        return std::numeric_limits<float>::infinity();  // in reality more like 1e18, but that's not signifcant and this could be optimized out to skip computation
    else if (phase>1)   // gas
        return gaseousViscosity;    // TODO could be reduced further with temperature (phase-1) above boiling, but not worth the computation probably 
    else 
        return liquidViscosity;     // TODO could also be scaled with temperature (phase) but could require heavy computation (exp) and extra parameters. Still a reduction to 10% or 1% @ 1.0 may be funny.       
}
}

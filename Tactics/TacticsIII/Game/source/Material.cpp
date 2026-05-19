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

/// @brief                          Name            Color, alpha = opacity(/255) per meter      freeze     boil(K)   Density  P(@boil)   molemass    Enthalpy  liq visc,  gas visc
//                                                                                                 (K)       (K)     (kg/m3)            (kg/mol)      (J/mol)    (Pa*s)   (Pa*s)
const Material Material::air    {   "air",          Engine::RGBA(0x40, 0x40, 0xF0, 0x0  ),        63.0f,    83.0f,   1026.0f,   870.0f,  28.695e-3f,   11570,    1e-4,    18e-6 };
const Material Material::water  {   "water",        Engine::RGBA(0x70, 0x90, 0xc0, 0xC0 ),       273.15f,  373.15f,   917.0f,  990.0f,   18.020e-3f,   40660,    1e-3,    12e-6 };
const Material Material::earth  {   "dirt",         Engine::RGBA(0x80, 0x40, 0x20       ),       800.0f,  1100.0f,   1600.0f,  2200.0f, 112.0e-3f,    168000,    1e+5,    20e-6 };
const Material Material::stone  {   "stone",        Engine::RGBA(0x60, 0x60, 0x60       ),      1250.0f,  3000.0f,   2700.0f,  2500.0f,  81.585e-3f,   49000,    1e+3,    20e-6 };
const Material Material::vegetation {"vegetation",  Engine::RGBA( 0x0, 0xA0,  0x0       ),       250.0f,   400.0f,   1100.0f,  1000.0f, 300.0e-3f,     72000,    1.0,     10e6 };
const Material Material::vacuum {   "vacuum",       Engine::RGBA( 0x0,  0x0,  0x0, 0x0  ),          NaN,      NaN,   0.0f,     0.0f,      0.0e-3f,       NaN,    0.0,     0.0 };
const Material* Material::all[] = { &air, &water, &earth, &stone, &vegetation };

// NB dirt behaves like rhyoitic lava when melted in visosity and enthalpy
// TODO: Oil (viscosity liquid 1, gas 10e-6), metal, wood, acid, poison, blood (viscosity 0.03)
// TODO viscosities (especially of gas) may need to be exagerated (eg x100 to x1000) to make it effective and visible. Such low viscosity will just get rounded down to 0.0. Alternatively just set these to 0 and ignore. It may not even be visible.
// Iron: boils at 3134K and enthalpy is 340kJ/mol 

float Material::PhaseFactor(float temperature) const
{
    return (temperature-freezingPoint)/(boilingPoint-freezingPoint);
}

float Material::Density(float temperature, float pressure) const
{
    // TODO very rough estimates, use ideal gas law and adjust liquids/solids as well with K value 
    float phase = PhaseFactor(temperature);
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
    float phase = PhaseFactor(temperature);
    if (phase<=0)   // solid
        return std::numeric_limits<float>::quiet_NaN(); 
    else if (phase>1)   // gas
        return (density / molarmass) * gasConstant * temperature;
    else 
        return atmosphericPressure * density / Density(temperature, atmosphericPressure);   
}


bool Material::IsSolid(float temperature) const
{
    return temperature <= freezingPoint;
}

bool Material::IsLiquid(float temperature) const
{
    return temperature > freezingPoint && temperature <= boilingPoint;
}

bool Material::IsGas(float temperature) const
{
    return temperature > boilingPoint;
}

float Material::Viscosity(float temperature) const
{
    float phase = PhaseFactor(temperature);
    if (phase<=0)   // solid
        return std::numeric_limits<float>::infinity();  // in reality more like 1e18, but that's not signifcant and this could be optimized out to skip computation
    else if (phase>1)   // gas
        return gaseousViscosity;    // TODO could be reduced further with temperature (phase-1) above boiling, but not worth the computation probably 
    else 
        return liquidViscosity;     // TODO could also be scaled with temperature (phase) but could require heavy computation (exp) and extra parameters. Still a reduction to 10% or 1% @ 1.0 may be funny.       
}
}

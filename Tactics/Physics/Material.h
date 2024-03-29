#pragma once
#include <string>
#include "Engine/Color.h"

namespace Physics
{

constexpr double IdealGasConstant = 8.31446261815324e3; // ideal gas constant in L * Pa / K * mol
constexpr double PascalPerAtmosphere = 101325.0;        // Pa/Atm


struct Material
{
    std::string_view name;
    Engine::RGBA color;
    double melt;    // kelvin
    double boil;    // kelvin
                    // double combust;
                    // texture
    double normalDensity;   // g/L at stp  0 �C, ~1000 mbar
    double molarMass;       // G/mol
    double viscosity;       // at normal pressure and room temperature (Pa*s = kgm-1s-1)
    double conductivity;    // J/Mol-K 
    double thermalConductivity; // W/mK = J/smK (at ~room temperature) https://en.wikipedia.org/wiki/List_of_thermal_conductivities 
    double heatCapacity;    // J/g*K    https://en.wikipedia.org/wiki/Table_of_specific_heat_capacities
    double surfaceTension;  // N/meter hardness when liquid https://en.wikipedia.org/wiki/Surface_tension#Data_table
    double youngsModulus;   // hardness when solid      (Pa) https://en.wikipedia.org/wiki/Young%27s_modulus#Approximate_values
    double granularity;     // particles per liter, 0 = totally solid 
    double opacity;         // mass attenuation coefficient of visible light (400-700nm) absorbed per voxel at 1.0 density
                            // ulatraviolet (sun light) would be 10-400nm 
                            // electric conductivity?

    double Density(double pressure, double temperature) const;     // pressure in pascal, temperature in kelvin, density in g/L
    double Viscosity(double density, double temperature) const;
    double Reynolds(double density, double temperature, double velocity = 1.0) const;
    bool Solid(double temperature) const;
    bool Fluid(double temperature) const;
    bool Gas(double temperature) const;
    bool Granular() const;
    // Evaporation rate in g/(second * m^2)
    double Evaporation(double temperature, double pressure=PascalPerAtmosphere, double humidity=0.001, double windSpeed=1) const; // g/m2
    Engine::RGBA Color(double temperature, double density) const;

    static const Material vacuum;
    static const Material air;
    static const Material soil;
    static const Material stone;
    static const Material water;
};

std::ostream& operator<<(std::ostream& stream, const Physics::Material& material);
std::ostream& operator<<(std::ostream& stream, const Physics::Material* material);


}
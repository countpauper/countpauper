#pragma once
#include <string>
#include "Element.h"
#include "Engine/Color.h"

namespace Game
{

struct Material
{
    std::wstring_view name;
    Element element;
    Engine::RGBA color;
    double melt;    // kelvin
    double boil;    // kelvin
                    // double combust;
                    // texture
    double normalDensity;   // g/L at stp  0 °C, ~1000 mbar
    double molarMass;       // G/mol
    double viscosity;       // mu, = N s /m2 = Pa*s t room temperature (25 C) 
    double conductivity;    // J/Mol-K 
    double heatCapacity;    // J/g*K    https://en.wikipedia.org/wiki/Table_of_specific_heat_capacities
    double surfaceTension;  // N/meter hardness when liquid https://en.wikipedia.org/wiki/Surface_tension#Data_table
    double youngsModulus;   // hardness when solid      (Pa) https://en.wikipedia.org/wiki/Young%27s_modulus#Approximate_values
    double opacity;         // mass attenuation coefficient of visible light (400-700nm) absorbed per voxel at 1.0 density
                            // ulatraviolet (sun light) would be 10-400nm 
                            // electric conductivity?


    double Density(double pressure, double temperature) const;

    static const Material vacuum;
    static const Material air;
    static const Material soil;
    static const Material stone;
    static const Material water;
};

constexpr double IdealGasConstant = 8.31446261815324e3; // ideal gas constant in L * Pa / K * mol
constexpr double PascalPerAtmosphere = 101325.0;        // Pa/Atm

}
#pragma once
#include "Rendering/Color.h"
#include <string>
namespace Game
{

static const float gasConstant = 8.31446261815324;  // J /(mol·K) 
static const float atmosphericPressure = 101325;    // Pa/Atm

struct Material
{
    std::string name;
    Engine::RGBA color;
    float freezingPoint;        // K
    float boilingPoint;         // K
    float solidDensity;         // kg/m3 constant if solid  
    float liquidDensity;        // kg/m3 at just below boiling, linear interpolate between solid density 
    float molarmass;            // g/mole
    float liquidViscosity;        
    float gaseousViscosity;

    float PhaseFactor(float temperature) const; // <=0 = solid >=1 gas, 0...1 liquid
    float Density(float temperature, float pressure = atmosphericPressure) const;
    bool IsSolid(float temperature) const;
    bool IsGas(float temperature) const;
    float Viscosity(float temperature) const;

    bool operator==(const Material& other) const;

    static const Material air;
    static const Material water;
    static const Material earth;
    static const Material stone;
    static const Material vegetation;
    static const Material vacuum;
    static const Material* all[5];
};


}

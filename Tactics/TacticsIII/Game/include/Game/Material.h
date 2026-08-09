#pragma once
#include "Rendering/Color.h"
#include <string>
namespace Game
{

static const float gasConstant = 8.31446261815324;  // J /(mol·K) 
static const float atmosphericPressure = 101325;    // Pa/Atm
static const float earthGravity =  9.80665;         // m/s^2
struct Material
{
    std::string name;
    Engine::RGBA color;
    float freezingPoint;        // K at 1 atmosphere
    float boilingPoint;         // K at 1 atmosphere
    float solidDensity;         // kg/m3 constant if solid  
    float liquidDensity;        // kg/m3 at just below boiling, linear interpolate between solid density 
    float molarmass;            // g/mole
    float enthalpy;             // J/mol 
    float liquidViscosity;        
    float gaseousViscosity;
    
    float PhaseFactor(float temperature, float pressure) const; // <=0 = solid >=1 gas, 0...1 liquid
    float Density(float temperature, float pressure = atmosphericPressure) const;
    float Pressure(float temperature, float density) const;

    bool IsSolid(float temperature, float pressure) const;
    bool IsLiquid(float temperature, float pressure) const;
    bool IsGas(float temperature, float pressure) const;
    float Viscosity(float temperature, float pressure) const;
    float FreezingPoint(float pressure) const;
    float BoilingPoint(float pressure) const;
    bool operator==(const Material& other) const;

    static const Material air;
    static const Material water;
    static const Material earth;
    static const Material stone;
    static const Material vegetation;
    static const Material vacuum;
    static const Material bedrock;
    static const Material* all[5];

private:
    float PhaseFactorSimple(float temperature) const;
};


}

#pragma once
#include "Rendering/Color.h"
#include <string>
namespace Game
{

struct Material
{
    std::string name;
    Engine::RGBA color;
    float freezingPoint; 
    float boilingPoint;
    float solidDensity;
    float liquidDensity;
    float gasDensity;
    
    float Density(float temperature) const;
    bool IsSolid(float temperature) const;
    bool IsGas(float temperature) const;

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

#include "Game/Material.h"
#include <limits>

namespace Game
{


bool Material::operator==(const Material& other) const
{
    return this == &other;
}

const Material Material::air { "air", Engine::RGBA(0x40, 0x40, 0xF0, 0x0),                63.0f,      83.0f,      0.9f,     0.9f,   1.225e-3 };
const Material Material::water { "water", Engine::RGBA(0x70, 0x90, 0xc0, 0xC0),         273.15f,    383.15f,    0.917f,     1.0f,   0.598e-3 };
const Material Material::earth { "ground", Engine::RGBA(0x80, 0x40, 0x20),               800.0f,    1100.0f,      1.5f,     1.0f,   1e-3f  };   // This is a mix of all (stone+water+vegetatio) so the boiling point etc is just wrong
const Material Material::stone { "stone", Engine::RGBA(0x60, 0x60, 0x60),               1250.0f,    3000.0f,      2.6f,     2.5f,   1e-3f };
const Material Material::vegetation { "vegetation", Engine::RGBA(0x0, 0xA0, 0x0),        250.0f,     400.0f,      0.5f,     0.5f,   1e-3f };
const Material Material::vacuum { "vacuum", Engine::RGBA(0x0, 0x0, 0x0, 0x0),             -1.0f,      -1.0f,      0.0f,     0.0f,   0.0f  };
const Material* Material::all[] = { &air, &water, &earth, &stone, &vegetation };


float Material::Density(float temperature) const
{
    // TODO very rough estimates, use ideal gas law and adjust liquids/solids as well with K value 
    if (IsSolid(temperature))
        return solidDensity; 
    else if (IsGas(temperature))
        return gasDensity;
    else    
        return liquidDensity;
}
bool Material::IsSolid(float temperature) const
{
    return temperature < freezingPoint;
}

bool Material::IsGas(float temperature) const
{
    return temperature > boilingPoint;
}
}

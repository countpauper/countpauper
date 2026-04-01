#include "Game/Material.h"

namespace Game
{


bool Material::operator==(const Material& other) const
{
    return this == &other;
}

const Material Material::air { "air", Engine::RGBA(0x40, 0x40, 0xF0, 0x0), 63.0f, 83.0f};
const Material Material::water { "water", Engine::RGBA(0x70, 0x90, 0xc0, 0xC0), 273.15f, 383.15f };
const Material Material::earth { "ground", Engine::RGBA(0x80, 0x40, 0x20), 800.0f, 1100.0f };   // This is a mix of all (stone+water+vegetatio) so the boiling point etc is just wrong
const Material Material::stone { "stone", Engine::RGBA(0x60, 0x60, 0x60), 1250.0f, 3000.0f };
const Material Material::vegetation { "vegetation", Engine::RGBA(0x0, 0xA0, 0x0), 250.0f, 400.0f };
const Material* Material::all[] = { &air, &water, &earth, &stone, &vegetation };


float Material::Density(float temperature) const
{
    // TODO very rough estimates, get actual physical values
    if (IsSolid(temperature))
        return 10.0; 
    else if (IsGas(temperature))
        return 0.1;
    else    
        return 1.0;
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

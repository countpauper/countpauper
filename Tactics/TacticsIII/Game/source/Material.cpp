#include "Game/Material.h"

namespace Game
{


bool Material::operator==(const Material& other) const
{
    return this == &other;
}

const Material Material::air { "air", Engine::RGBA(0x40, 0x40, 0xF0, 0x0) };
const Material Material::water { "water", Engine::RGBA(0x70, 0x90, 0xc0, 0x80) };
const Material Material::earth { "ground", Engine::RGBA(0x80, 0x40, 0x20) };
const Material Material::stone { "stone", Engine::RGBA(0x60, 0x60, 0x60) };
const Material Material::vegetation { "vegetation", Engine::RGBA(0x0, 0xA0, 0x0) };
const Material* Material::all[] = { &air, &water, &earth, &stone, &vegetation };

}

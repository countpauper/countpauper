#include "Game/Material.h"

namespace Game
{

const Material Material::air { "air", Engine::RGBA(0x40, 0x40, 0xF0, 0x0) };
const Material Material::earth { "ground", Engine::RGBA(0x80, 0x40, 0x20) };
const Material Material::stone { "stone", Engine::RGBA(0x60, 0x60, 0x60) };
const Material Material::grass { "grass", Engine::RGBA(0x0, 0xA0, 0x0) }; // TODO: instead vegation layer with grass, flowers, grain, brush, bush, vines, other stuff
const Material* Material::all[] = { &air, &earth, &stone, &grass };

}

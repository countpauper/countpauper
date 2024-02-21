#pragma once
#include "Rendering/Color.h"
#include <string>
namespace Game
{

struct Material
{
    std::string name;
    Engine::RGBA color;

    static const Material air;
    static const Material earth;
    static const Material stone;
    static const Material grass;
    static const Material* all[4];
};


}

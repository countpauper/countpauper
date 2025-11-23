#pragma once
#include "Rendering/Color.h"
#include <string>
namespace Game
{

struct Material
{
    std::string name;
    Engine::RGBA color;

    bool operator==(const Material& other) const;

    static const Material air;
    static const Material water;
    static const Material earth;
    static const Material stone;
    static const Material vegetation;
    static const Material* all[5];
};


}

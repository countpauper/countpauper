#pragma once
#include "Engine/Size.h"
#include "Game/Material.h"
#include <vector>

namespace Engine
{
    class Image;
};

namespace Game
{
class Map
{
public:
    explicit Map(Engine::Size size);
    Map(const Engine::Image& data, int height);
private:
    struct Grid
    {
        int level = 0;
        const Material* ground = &Material::earth;
        int liquidity = 0;
        const Material* liquid = nullptr;
        const Material* gas = nullptr;
    };

    Engine::Size size;
    std::vector<Grid> grids;
};

}

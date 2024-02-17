#include "Game/Map.h"
#include "Engine/Image.h"
#include "Engine/Geometry.h"
#include "Engine/Debug.h"
#include "Game/Material.h"

namespace Game
{

Map::Map(Engine::Size size) :
    size(size),
    grids(size.Volume())
{
}

const Material* FindMaterial(Engine::HSVA color)
{
    if (color.Value() == 0 )
    {
        return nullptr;
    }
    float nearest = std::numeric_limits<float>::max();
    const Material* result = nullptr;
    float findHue = Engine::Deg2Rad(color.Hue());
    // TODO: std::span would be nicer here but C++20 doesn't run in the debugger
    for(int i =0; i< sizeof(Material::all)/sizeof(Material::all[0]); ++i)
    {
        const Material& material = *Material::all[i];
        Engine::HSVA materialColor(material.color);
        float distance = std::abs(Engine::ShortestTurn(findHue, Engine::Deg2Rad(materialColor.Hue()))) +
            std::abs(color.Saturation() - materialColor.Saturation());
        if (distance < nearest)
        {
            nearest = distance;
            result = &material;
        }
    }
    return result;
}

Map::Map(const Engine::Image& data, int height) :
    Map(Engine::Size{int(data.Width()), int(data.Height()/4), height})
{
    for(unsigned y=0; y<size.y; ++y)
    {
        for(unsigned x=0; x< size.x; ++x)
        {
            Grid& grid = grids[y*size.x + x];
            Engine::RGBA levelPixel = data[Engine::Position(x, y)];
            Engine::HSVA materialPixel(data[Engine::Position(x, y + size.y)]);
            Engine::HSVA liquidPixel(data[Engine::Position(x, y + 2 * size.y)]);
            Engine::HSVA gasPixel(data[Engine::Position(x, y+ 3*size.y)]);
            grid.level = height *  levelPixel.r / 255;
            grid.liquidity = (height * levelPixel.b / 255) - grid.level;
            // TODO: green is gas level
            grid.ground = FindMaterial(materialPixel);

            grid.liquid = FindMaterial(Engine::HSVA(liquidPixel));
            grid.gas = FindMaterial(Engine::HSVA(gasPixel));

            /*
            Engine::Debug::Log("Pixel [%d, %d] %f %f %f is %s. %d solid + %d liquid", x,y,
                materialPixel.Hue(), 100.0 * materialPixel.Saturation(), 100.0 * materialPixel.Value(), grid.ground->name.c_str(),
                grid.level, grid.liquidity);
            */
        }
    }
}

}

#include "Game/Map.h"
#include "Engine/Image.h"
#include "Engine/Geometry.h"
#include "Engine/Debug.h"
#include "Engine/Maths.h"
#include "Game/Material.h"
#include <cassert>

namespace Game
{

Map::Map(Engine::Size size) :
    size(size),
    grids(size.x * size.y)
{
    GenerateMesh();
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
    size{int(data.Width()), int(data.Height()/4), height},
    grids(size.x * size.y)
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
    GenerateMesh();
}

std::string_view Map::Name() const
{
    return "map";
}

Engine::Mesh& Map::GetMesh()
{
    return mesh;
}

Map::Grid& Map::operator[](Position pos)
{
    return grids[pos.x + pos.y * size.x];
}

const Map::Grid& Map::operator[](Position pos) const
{
    return grids[pos.x + pos.y * size.x];
}

Engine::Coordinate Map::GroundCoord(Position pos) const
{
    auto& grid = (*this)[pos];
    return Engine::Coordinate(
        pos.x + 0.5,
        pos.y + 0.5,
        grid.level
    );
}

void Map::GenerateMesh()
{
    int idx = 0;
    Engine::Vector up(0, 0, 1);

    for(const auto& grid: grids)
    {
        int x = idx % size.x;
        int y = idx / size.x;

        unsigned vertidx = mesh.vertices.size();
        Engine::Quad quad(
            Engine::Coordinate(x, y, grid.level),
            Engine::Coordinate(x+1, y, grid.level),
            Engine::Coordinate(x+1, y+1, grid.level),
            Engine::Coordinate(x, y+1, grid.level)
        );
        float brightness = grid.level / float(size.z);
        brightness = 0.2 + brightness*0.8;
        auto groundColor = grid.ground->color;
        auto vertexColor = Engine::Lerp(Engine::RGBA::black, groundColor, brightness);
        quad.SetColor(vertexColor);
        quad.SetName(idx);
        mesh += quad;

        if (x > 0 )
        {
            Grid& neighbourGrid = grids[idx-1];
            if (neighbourGrid.level != grid.level)
            {
                // TODO add as a quad here too with its own normal and texture coordinates when adding light,
                //  then fake lighting can be removed as that's the complication now to recompute
                unsigned neighbourVertexIdx = vertidx - 4;
                mesh.triangles.push_back({vertidx, vertidx+3, neighbourVertexIdx +1});
                mesh.names.push_back(idx);
                mesh.triangles.push_back({vertidx+3, neighbourVertexIdx +2, neighbourVertexIdx +1});
                mesh.names.push_back(idx);
            }
        }
        if (y >0 )
        {
            Grid& neighbourGrid = grids[idx - size.x];
            if (neighbourGrid.level != grid.level)
            {
                unsigned neighbourVertexIdx = vertidx - (4 * size.x);
                mesh.triangles.push_back({vertidx+1, vertidx+0, neighbourVertexIdx +3});
                mesh.names.push_back(idx);
                mesh.triangles.push_back({vertidx+1, neighbourVertexIdx+3, neighbourVertexIdx +2});
                mesh.names.push_back(idx);
            }
        }

        ++idx;
    }
    assert(mesh.names.size() == mesh.triangles.size());
}

}

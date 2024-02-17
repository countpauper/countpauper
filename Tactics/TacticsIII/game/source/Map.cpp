#include "Game/Map.h"
#include "Engine/Image.h"
#include "Engine/Geometry.h"
#include "Engine/Debug.h"
#include "Game/Material.h"

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

Engine::Mesh& Map::GetMesh()
{
    return mesh;
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
        mesh.vertices.emplace_back(Engine::Mesh::Vertex{
            Engine::Coordinate(x, y, grid.level),
            up,
            Engine::Mesh::TextureCoordinate{0, 0, 0},
            grid.ground->color
        });

        mesh.vertices.emplace_back(Engine::Mesh::Vertex{
            Engine::Coordinate(x+1, y, grid.level),
            up,
            Engine::Mesh::TextureCoordinate{1, 0, 0},
            grid.ground->color
        });

        mesh.vertices.emplace_back(Engine::Mesh::Vertex{
            Engine::Coordinate(x+1, y+1, grid.level),
            up,
            Engine::Mesh::TextureCoordinate{1, 1, 0},
            grid.ground->color
        });

        mesh.vertices.emplace_back(Engine::Mesh::Vertex{
            Engine::Coordinate(x, y+1, grid.level),
            up,
            Engine::Mesh::TextureCoordinate{0, 1, 0},
            grid.ground->color
        });

        mesh.triangles.push_back({vertidx, vertidx+1, vertidx +2});
        mesh.triangles.push_back({vertidx, vertidx+2, vertidx +3});

        if (x > 0 )
        {
            Grid& neighbourGrid = grids[idx-1];
            unsigned neighbourVertexIdx = vertidx - 4;
            mesh.triangles.push_back({vertidx, vertidx+3, neighbourVertexIdx +1});
            mesh.triangles.push_back({vertidx+3, neighbourVertexIdx +2, neighbourVertexIdx +1});
        }
        if (y >0 )
        {
            Grid& neighbourGrid = grids[idx - size.x];
            unsigned neighbourVertexIdx = vertidx - (4 * size.x);
            mesh.triangles.push_back({vertidx+1, vertidx+0, neighbourVertexIdx +3});
            mesh.triangles.push_back({vertidx+1, neighbourVertexIdx+3, neighbourVertexIdx +2});
        }
        Engine::Debug::Log("Grid[%d] added at (%d, %d)", idx, x, y);

        ++idx;
    }
}

}

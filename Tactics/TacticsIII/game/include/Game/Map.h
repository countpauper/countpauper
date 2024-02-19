#pragma once
#include "Engine/Size.h"
#include "Engine/Mesh.h"
#include "Engine/Object.h"
#include "Game/Material.h"
#include <vector>

namespace Engine
{
    class Image;
};

namespace Game
{
class Map : public Engine::Object
{
public:
    struct Position
    {
        int x, y;
    };
    explicit Map(Engine::Size size);
    Map(const Engine::Image& data, int height);
    std::string_view Name() const override;
    Engine::Mesh& GetMesh();
    Engine::Coordinate GroundCoord(Position pos) const;
private:
    struct Grid
    {
        int level = 0;
        const Material* ground = &Material::earth;
        int liquidity = 0;
        const Material* liquid = nullptr;
        const Material* gas = nullptr;
    };

    void GenerateMesh();
    Grid& operator[](Position pos);
    const Grid& operator[](Position pos) const;
    Engine::Size size;
    std::vector<Grid> grids;
    Engine::Mesh mesh;
};

}
